#include "storage/storage.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace Storage
{
    std::shared_ptr<Storage_SQL> Storage_SQL::s_instance = nullptr;
    std::mutex Storage_SQL::s_mutex;

    namespace
    {
        std::string findProjectFile(const std::string &relative_path)
        {
            namespace fs = std::filesystem;
            const fs::path target(relative_path);
            const fs::path current = fs::current_path();

            for (const auto &base : {current, current.parent_path()})
            {
                const auto candidate = base / target;
                if (fs::exists(candidate))
                {
                    return fs::absolute(candidate).lexically_normal().string();
                }
            }

            return target.string();
        }

        json columnToJson(sqlite3_stmt *stmt, int index)
        {
            switch (sqlite3_column_type(stmt, index))
            {
            case SQLITE_INTEGER:
                return sqlite3_column_int64(stmt, index);
            case SQLITE_FLOAT:
                return sqlite3_column_double(stmt, index);
            case SQLITE_NULL:
                return nullptr;
            case SQLITE_TEXT:
            case SQLITE_BLOB:
            default:
            {
                const auto *text = sqlite3_column_text(stmt, index);
                return text ? reinterpret_cast<const char *>(text) : "";
            }
            }
        }
    }

    Storage_SQL::~Storage_SQL()
    {
        std::lock_guard<std::mutex> lock(db_mutex);
        if (db)
        {
            sqlite3_close(db);
            db = nullptr;
        }
    }

    bool Storage_SQL::readSQLFile(const std::string &file_path, std::string &sql_content)
    {
        std::ifstream file(file_path);
        if (!file.is_open())
        {
            std::cerr << "Failed to open SQL file: " << file_path << std::endl;
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        sql_content = buffer.str();

        if (sql_content.empty())
        {
            std::cerr << "SQL file is empty: " << file_path << std::endl;
            return false;
        }

        return true;
    }

    void Storage_SQL::execSQL(const std::string &sql)
    {
        if (!db)
        {
            std::cerr << "Database not initialized" << std::endl;
            return;
        }

        char *errMsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

        if (rc != SQLITE_OK)
        {
            std::cerr << "SQL error: "
                      << (errMsg ? errMsg : "unknown error")
                      << std::endl;

            sqlite3_free(errMsg);
        }
    }



    bool Storage_SQL::init_db()
    {
        auto config = Config::AppConfig::Instance();
        std::string db_path = config->getDatabasePath();
        if (db_path.empty())
        {
            std::cerr << "Database path is empty" << std::endl;
            return false;
        }
        std::filesystem::path db_file_path(db_path);
        std::error_code ec;

        auto parent_path = db_file_path.parent_path();
        if (!parent_path.empty() && !std::filesystem::exists(parent_path, ec))
        {
            if (!std::filesystem::create_directories(parent_path, ec))
            {
                std::cerr << "Failed to create database directory: "
                          << parent_path
                          << ", error: "
                          << ec.message()
                          << std::endl;
                return false;
            }
        }

        bool db_exists = std::filesystem::exists(db_file_path, ec);
        if (ec)
        {
            std::cerr << "Failed to check database file: "
                      << db_file_path
                      << ", error: "
                      << ec.message()
                      << std::endl;
            return false;
        }

        bool is_empty_db = false;
        if (db_exists)
        {
            auto size = std::filesystem::file_size(db_file_path, ec);
            if (ec)
            {
                std::cerr << "Failed to get database file size: "
                          << db_file_path
                          << ", error: "
                          << ec.message()
                          << std::endl;
                return false;
            }

            is_empty_db = (size == 0);
        }

        bool need_init = !db_exists || is_empty_db;

        int rc = sqlite3_open(db_path.c_str(), &db);
        if (rc != SQLITE_OK)
        {
            std::cerr << "Cannot open database: "
                      << (db ? sqlite3_errmsg(db) : "unknown error")
                      << std::endl;

            if (db)
            {
                sqlite3_close(db);
                db = nullptr;
            }

            return false;
        }

        execSQL("PRAGMA foreign_keys = ON;");
        need_init = need_init || !tableExists("products");

        if (need_init)
        {
            std::string sql_content;
            if (!readSQLFile(findProjectFile(config->getSqlFile()), sql_content))
            {
                std::cerr << "Failed to read migration file" << std::endl;
                return false;
            }

            char *errMsg = nullptr;
            rc = sqlite3_exec(db, sql_content.c_str(), nullptr, nullptr, &errMsg);

            if (rc != SQLITE_OK)
            {
                std::cerr << "SQL initialization error: "
                          << (errMsg ? errMsg : "unknown error")
                          << std::endl;

                sqlite3_free(errMsg);
                return false;
            }
            std::cout << "Database initialized successfully" << std::endl;
        }
        else
        {
            std::cout << "Database already exists, skipping initialization" << std::endl;
        }
        return ensurePortfolioProjectSchema();
    }

    std::shared_ptr<Storage_SQL> Storage_SQL::Instance()
    {
        std::lock_guard<std::mutex> lock(s_mutex);

        if (!s_instance)
        {
            auto instance = std::shared_ptr<Storage_SQL>(new Storage_SQL());

            if (!instance->init_db())
            {
                std::cerr << "Failed to initialize database" << std::endl;
                return nullptr;
            }

            s_instance = instance;
        }

        return s_instance;
    }

    bool Storage_SQL::tableExists(const std::string &table_name)
    {
        const auto result = queryOne(
            "SELECT name FROM sqlite_master WHERE type = 'table' AND name = ?;",
            {table_name});
        return !result.empty();
    }

    bool Storage_SQL::columnExists(const std::string &table_name, const std::string &column_name)
    {
        const auto rows = queryRows("PRAGMA table_info(\"" + table_name + "\");");
        for (const auto &row : rows)
        {
            if (row.contains("name") && row["name"].is_string() && row["name"].get<std::string>() == column_name)
            {
                return true;
            }
        }
        return false;
    }

    bool Storage_SQL::ensurePortfolioProjectSchema()
    {
        if (!tableExists("products"))
        {
            std::cerr << "products table is missing" << std::endl;
            return false;
        }

        const std::vector<std::string> statements = {
            "CREATE TABLE IF NOT EXISTS schema_migrations ("
            "version TEXT PRIMARY KEY, "
            "applied_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP, "
            "note TEXT NOT NULL DEFAULT ''"
            ");",

            "CREATE TABLE IF NOT EXISTS site_profile ("
            "id INTEGER PRIMARY KEY CHECK(id = 1), "
            "owner_name TEXT NOT NULL DEFAULT 'Xiang Y.', "
            "site_name TEXT NOT NULL DEFAULT 'Personal Software Lab', "
            "subtitle TEXT NOT NULL DEFAULT 'Self-built apps, tools, systems, and experiments.', "
            "hero_label TEXT NOT NULL DEFAULT 'DEVELOPER & BUILDER', "
            "hero_title TEXT NOT NULL DEFAULT 'Personal Software Lab', "
            "hero_description TEXT NOT NULL DEFAULT 'A collection of self-built apps, tools, systems, and experiments — crafted with code and curiosity.', "
            "github_url TEXT NOT NULL DEFAULT '', "
            "email TEXT NOT NULL DEFAULT '', "
            "resume_url TEXT NOT NULL DEFAULT '', "
            "linkedin_url TEXT NOT NULL DEFAULT '', "
            "twitter_url TEXT NOT NULL DEFAULT '', "
            "created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP, "
            "updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP"
            ");",

            "CREATE TABLE IF NOT EXISTS portfolio_projects ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "product_id INTEGER, "
            "slug TEXT NOT NULL UNIQUE, "
            "name TEXT NOT NULL, "
            "category TEXT NOT NULL, "
            "description TEXT NOT NULL DEFAULT '', "
            "long_description TEXT NOT NULL DEFAULT '', "
            "featured INTEGER NOT NULL DEFAULT 0, "
            "status TEXT NOT NULL DEFAULT 'Live', "
            "year TEXT NOT NULL DEFAULT '', "
            "platform TEXT NOT NULL DEFAULT '', "
            "role TEXT NOT NULL DEFAULT '', "
            "project_type TEXT NOT NULL DEFAULT '', "
            "cover_image_url TEXT NOT NULL DEFAULT '', "
            "hero_image_url TEXT NOT NULL DEFAULT '', "
            "tech_stack_json TEXT NOT NULL DEFAULT '[]', "
            "features_json TEXT NOT NULL DEFAULT '[]', "
            "screenshots_json TEXT NOT NULL DEFAULT '[]', "
            "architecture_json TEXT NOT NULL DEFAULT '[]', "
            "challenge TEXT NOT NULL DEFAULT '', "
            "solution TEXT NOT NULL DEFAULT '', "
            "result TEXT NOT NULL DEFAULT '', "
            "links_json TEXT NOT NULL DEFAULT '{}', "
            "visibility TEXT NOT NULL DEFAULT 'public', "
            "sort_order INTEGER NOT NULL DEFAULT 100, "
            "created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP, "
            "updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP, "
            "FOREIGN KEY(product_id) REFERENCES products(id), "
            "CHECK(featured IN (0, 1)), "
            "CHECK(visibility IN ('public', 'hidden'))"
            ");"};

        for (const auto &statement : statements)
        {
            if (!executeStatement(statement))
            {
                return false;
            }
        }

        const std::vector<std::string> project_indexes = {
            "CREATE INDEX IF NOT EXISTS idx_portfolio_projects_public_order "
            "ON portfolio_projects(visibility, sort_order, id);",
            "CREATE INDEX IF NOT EXISTS idx_portfolio_projects_category "
            "ON portfolio_projects(category, visibility, sort_order);"};

        for (const auto &statement : project_indexes)
        {
            if (!executeStatement(statement))
            {
                return false;
            }
        }

        if (!columnExists("products", "github_url") &&
            !executeStatement("ALTER TABLE products ADD COLUMN github_url TEXT NOT NULL DEFAULT '';"))
        {
            return false;
        }

        const std::vector<std::pair<std::string, std::string>> site_columns = {
            {"owner_name", "TEXT NOT NULL DEFAULT 'Xiang Y.'"},
            {"hero_label", "TEXT NOT NULL DEFAULT 'DEVELOPER & BUILDER'"},
            {"hero_title", "TEXT NOT NULL DEFAULT 'Personal Software Lab'"},
            {"hero_description", "TEXT NOT NULL DEFAULT 'A collection of self-built apps, tools, systems, and experiments — crafted with code and curiosity.'"},
            {"resume_url", "TEXT NOT NULL DEFAULT ''"},
            {"linkedin_url", "TEXT NOT NULL DEFAULT ''"},
            {"twitter_url", "TEXT NOT NULL DEFAULT ''"}};

        for (const auto &[column, definition] : site_columns)
        {
            if (!columnExists("site_profile", column) &&
                !executeStatement("ALTER TABLE site_profile ADD COLUMN " + column + " " + definition + ";"))
            {
                return false;
            }
        }

        if (tableExists("releases") && !columnExists("releases", "html_path") &&
            !executeStatement("ALTER TABLE releases ADD COLUMN html_path TEXT;"))
        {
            return false;
        }

        if (tableExists("releases") && columnExists("releases", "html_path") && columnExists("releases", "vue_path") &&
            !executeStatement("UPDATE releases SET html_path = vue_path WHERE (html_path IS NULL OR html_path = '') AND vue_path IS NOT NULL AND vue_path <> '';"))
        {
            return false;
        }

        if (tableExists("releases") && columnExists("releases", "html_path") &&
            !executeStatement("UPDATE releases SET html_path = substr(html_path, 1, length(html_path) - 4) || '.html' WHERE html_path LIKE '%.vue';"))
        {
            return false;
        }

        if (tableExists("releases") && columnExists("releases", "html_path") &&
            !executeStatement("UPDATE releases SET html_path = replace(html_path, '/vue/', '/html/') WHERE html_path LIKE '%/vue/%';"))
        {
            return false;
        }

        if (!executeStatement(
                "INSERT INTO site_profile ("
                "id, owner_name, site_name, subtitle, hero_label, hero_title, hero_description, "
                "github_url, email, resume_url, linkedin_url, twitter_url"
                ") VALUES ("
                "1, 'Xiang Y.', 'Personal Software Lab', "
                "'Self-built apps, tools, systems, and experiments.', "
                "'DEVELOPER & BUILDER', 'Personal Software Lab', "
                "'A collection of self-built apps, tools, systems, and experiments — crafted with code and curiosity.', "
                "'', '', '', '', ''"
                ") "
                "ON CONFLICT(id) DO NOTHING;"))
        {
            return false;
        }

        if (!executeStatement(
                "UPDATE site_profile "
                "SET owner_name = 'Xiang Y.', "
                "    site_name = 'Personal Software Lab', "
                "    subtitle = 'Self-built apps, tools, systems, and experiments.', "
                "    hero_label = 'DEVELOPER & BUILDER', "
                "    hero_title = 'Personal Software Lab', "
                "    hero_description = 'A collection of self-built apps, tools, systems, and experiments — crafted with code and curiosity.' "
                "WHERE id = 1 "
                "  AND site_name = 'YXX Works' "
                "  AND subtitle = '产品、插件与创作实验';"))
        {
            return false;
        }

        if (!executeStatement(
                "DELETE FROM portfolio_projects "
                "WHERE product_id IS NULL "
                "  AND slug IN ("
                "'insightboard', 'autoreport', 'datacanvas', 'promptflow', 'taskpilot', "
                "'codesnip', 'serverlens', 'notestream', 'datapulse', 'shipgate'"
                ");"))
        {
            return false;
        }

        return executeStatement(
            "INSERT INTO schema_migrations (version, note) "
            "VALUES ('20260607_portfolio_projects', 'portfolio project schema') "
            "ON CONFLICT(version) DO NOTHING;");
    }

    json Storage_SQL::queryRows(const std::string &sql, const std::vector<std::string> &params)
    {
        std::lock_guard<std::mutex> lock(db_mutex);
        json rows = json::array();

        if (!db)
        {
            std::cerr << "Database not initialized" << std::endl;
            return rows;
        }

        sqlite3_stmt *stmt = nullptr;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            std::cerr << "SQL prepare error: " << sqlite3_errmsg(db) << "\nSQL: " << sql << std::endl;
            return rows;
        }

        for (std::size_t i = 0; i < params.size(); ++i)
        {
            sqlite3_bind_text(stmt, static_cast<int>(i + 1), params[i].c_str(), -1, SQLITE_TRANSIENT);
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            json row = json::object();
            const int column_count = sqlite3_column_count(stmt);
            for (int i = 0; i < column_count; ++i)
            {
                const char *name = sqlite3_column_name(stmt, i);
                row[name ? name : ""] = columnToJson(stmt, i);
            }
            rows.push_back(row);
        }

        if (rc != SQLITE_DONE)
        {
            std::cerr << "SQL step error: " << sqlite3_errmsg(db) << "\nSQL: " << sql << std::endl;
        }

        sqlite3_finalize(stmt);
        return rows;
    }

    json Storage_SQL::queryOne(const std::string &sql, const std::vector<std::string> &params)
    {
        auto rows = queryRows(sql, params);
        if (!rows.is_array() || rows.empty())
        {
            return json::object();
        }
        return rows.front();
    }

    bool Storage_SQL::executeStatement(const std::string &sql, const std::vector<std::string> &params)
    {
        std::lock_guard<std::mutex> lock(db_mutex);

        if (!db)
        {
            std::cerr << "Database not initialized" << std::endl;
            return false;
        }

        sqlite3_stmt *stmt = nullptr;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            std::cerr << "SQL prepare error: " << sqlite3_errmsg(db) << "\nSQL: " << sql << std::endl;
            return false;
        }

        for (std::size_t i = 0; i < params.size(); ++i)
        {
            sqlite3_bind_text(stmt, static_cast<int>(i + 1), params[i].c_str(), -1, SQLITE_TRANSIENT);
        }

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE)
        {
            std::cerr << "SQL execute error: " << sqlite3_errmsg(db) << "\nSQL: " << sql << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_finalize(stmt);
        return true;
    }

}
