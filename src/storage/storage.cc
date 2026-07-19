#include "storage/storage.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string_view>

namespace Storage
{
std::shared_ptr<Storage_SQL> Storage_SQL::s_instance = nullptr;
std::mutex Storage_SQL::s_mutex;

namespace
{
namespace fs = std::filesystem;

std::string findProjectFile(const std::string &relative_path)
{
    const fs::path target(relative_path);
    if (target.is_absolute() && fs::exists(target))
    {
        return target.lexically_normal().string();
    }

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

json parseJsonField(const json &row, const std::string &key, const json &fallback)
{
    if (!row.contains(key) || row[key].is_null())
    {
        return fallback;
    }

    try
    {
        if (row[key].is_string())
        {
            const auto raw = row[key].get<std::string>();
            return raw.empty() ? fallback : json::parse(raw);
        }
        return row[key];
    }
    catch (const json::parse_error &)
    {
        return fallback;
    }
}

std::string optionalString(const json &row, const std::string &key)
{
    if (!row.contains(key) || row[key].is_null())
    {
        return "";
    }
    if (row[key].is_string())
    {
        return row[key].get<std::string>();
    }
    return row[key].dump();
}

bool hasUrlScheme(std::string_view value)
{
    return value.rfind("http://", 0) == 0 ||
           value.rfind("https://", 0) == 0 ||
           value.rfind("/", 0) == 0 ||
           value.rfind("mailto:", 0) == 0;
}

std::string assetUrl(const std::string &asset_key)
{
    if (asset_key.empty() || hasUrlScheme(asset_key))
    {
        return asset_key;
    }
    return "/assets/" + asset_key;
}

json normalizeScreenshots(json screenshots)
{
    if (!screenshots.is_array())
    {
        return json::array();
    }

    for (auto &item : screenshots)
    {
        if (item.is_object() && item.contains("image") && item["image"].is_string())
        {
            item["image"] = assetUrl(item["image"].get<std::string>());
        }
    }
    return screenshots;
}

json normalizePage(json row)
{
    row["sortOrder"] = row.value("sortOrder", 100);
    row["htmlUrl"] = "/projects/" + optionalString(row, "projectSlug") + "/pages/" + optionalString(row, "slug");
    row.erase("html_asset_key");
    return row;
}

json normalizeProject(json row)
{
    row["featured"] = row.value("featured", 0) == 1;
    row["description"] = optionalString(row, "summary");
    row["longDescription"] = optionalString(row, "description_long");
    row["type"] = optionalString(row, "project_type");
    row["coverImageUrl"] = assetUrl(optionalString(row, "cover_asset_key"));
    row["heroImageUrl"] = assetUrl(optionalString(row, "hero_asset_key"));
    row["techStack"] = parseJsonField(row, "tech_stack_json", json::array());
    row["features"] = parseJsonField(row, "features_json", json::array());
    row["screenshots"] = normalizeScreenshots(parseJsonField(row, "screenshots_json", json::array()));
    row["architecture"] = parseJsonField(row, "architecture_json", json::array());
    row["links"] = parseJsonField(row, "links_json", json::object());
    row["sortOrder"] = row.value("sort_order", 100);

    row.erase("description_long");
    row.erase("project_type");
    row.erase("cover_asset_key");
    row.erase("hero_asset_key");
    row.erase("tech_stack_json");
    row.erase("features_json");
    row.erase("screenshots_json");
    row.erase("architecture_json");
    row.erase("links_json");
    row.erase("sort_order");
    return row;
}

std::string projectSelect()
{
    return
        "SELECT "
        "slug, name, category, summary, description AS description_long, status, year, role, platform, "
        "project_type, cover_asset_key, hero_asset_key, tech_stack_json, features_json, screenshots_json, "
        "architecture_json, links_json, featured, sort_order "
        "FROM projects ";
}

bool isSafeRelativePath(const fs::path &relative_path)
{
    if (relative_path.empty() || relative_path.is_absolute())
    {
        return false;
    }

    for (const auto &part : relative_path)
    {
        if (part == ".." || part == ".")
        {
            return false;
        }
    }
    return true;
}

fs::path absoluteConfiguredPath(const std::string &raw_path)
{
    const fs::path path(raw_path);
    if (path.is_absolute())
    {
        return path.lexically_normal();
    }
    return fs::absolute(path).lexically_normal();
}
}  // namespace

Storage_SQL::~Storage_SQL()
{
    std::lock_guard<std::mutex> lock(db_mutex);
    if (db)
    {
        sqlite3_close(db);
        db = nullptr;
    }
}

std::shared_ptr<Storage_SQL> Storage_SQL::Instance()
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (!s_instance)
    {
        auto instance = std::shared_ptr<Storage_SQL>(new Storage_SQL());
        if (!instance->initDb())
        {
            std::cerr << "Failed to initialize showcase database" << std::endl;
            return nullptr;
        }
        s_instance = instance;
    }
    return s_instance;
}

bool Storage_SQL::initDb()
{
    const auto config = Config::AppConfig::Instance();
    const auto db_path = config->getDatabasePath();
    if (db_path.empty())
    {
        std::cerr << "Database path is empty" << std::endl;
        return false;
    }

    std::error_code ec;
    const fs::path db_file_path(db_path);
    const auto parent_path = db_file_path.parent_path();
    if (!parent_path.empty())
    {
        fs::create_directories(parent_path, ec);
        if (ec)
        {
            std::cerr << "Failed to create database directory: " << ec.message() << std::endl;
            return false;
        }
    }

    const int rc = sqlite3_open(db_path.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Cannot open database: " << (db ? sqlite3_errmsg(db) : "unknown error") << std::endl;
        if (db)
        {
            sqlite3_close(db);
            db = nullptr;
        }
        return false;
    }

    executeSqlScript("PRAGMA foreign_keys = ON; PRAGMA journal_mode = WAL; PRAGMA synchronous = NORMAL;");
    sqlite3_busy_timeout(db, config->getSqliteBusyTimeoutMs());

    return runMigrations();
}

bool Storage_SQL::runMigrations()
{
    if (tableExists("projects") && tableExists("site_profile"))
    {
        return true;
    }

    std::string sql_content;
    if (!readSqlFile(findProjectFile(Config::AppConfig::Instance()->getSqlFile()), sql_content))
    {
        return false;
    }
    return executeSqlScript(sql_content);
}

bool Storage_SQL::readSqlFile(const std::string &file_path, std::string &sql_content)
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
    return !sql_content.empty();
}

bool Storage_SQL::executeSqlScript(const std::string &sql)
{
    std::lock_guard<std::mutex> lock(db_mutex);
    if (!db)
    {
        return false;
    }

    char *err_msg = nullptr;
    const int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL script error: " << (err_msg ? err_msg : "unknown error") << std::endl;
        sqlite3_free(err_msg);
        return false;
    }
    return true;
}

bool Storage_SQL::executeStatement(const std::string &sql, const std::vector<std::string> &params)
{
    std::lock_guard<std::mutex> lock(db_mutex);
    if (!db)
    {
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

json Storage_SQL::queryRows(const std::string &sql, const std::vector<std::string> &params)
{
    std::lock_guard<std::mutex> lock(db_mutex);
    json rows = json::array();
    if (!db)
    {
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
    const auto rows = queryRows(sql, params);
    if (!rows.is_array() || rows.empty())
    {
        return json::object();
    }
    return rows.front();
}

bool Storage_SQL::tableExists(const std::string &table_name)
{
    const auto result = queryOne(
        "SELECT name FROM sqlite_master WHERE type = 'table' AND name = ?;",
        {table_name});
    return !result.empty();
}

json Storage_SQL::getSiteProfile()
{
    auto profile = queryOne(
        "SELECT "
        "owner_name AS ownerName, site_name AS siteName, subtitle, hero_label AS heroLabel, "
        "hero_title AS heroTitle, hero_description AS heroDescription, github_url AS githubUrl, "
        "email, resume_url AS resumeUrl, linkedin_url AS linkedinUrl, twitter_url AS twitterUrl "
        "FROM site_profile WHERE id = 1 LIMIT 1;");

    if (!profile.empty())
    {
        return profile;
    }

    return {
        {"ownerName", "Xiang Y."},
        {"siteName", "Personal Software Lab"},
        {"subtitle", "Self-built apps, tools, systems, and experiments."},
        {"heroLabel", "DEVELOPER & BUILDER"},
        {"heroTitle", "Personal Software Lab"},
        {"heroDescription", "A collection of self-built apps, tools, systems, and experiments."},
        {"githubUrl", ""},
        {"email", ""},
        {"resumeUrl", ""},
        {"linkedinUrl", ""},
        {"twitterUrl", ""}};
}

json Storage_SQL::getProjectList()
{
    auto rows = queryRows(
        projectSelect() +
        "WHERE visibility = 'public' "
        "ORDER BY sort_order ASC, id ASC;");

    json items = json::array();
    for (auto &row : rows)
    {
        items.push_back(normalizeProject(row));
    }
    return items;
}

json Storage_SQL::getHomeData()
{
    auto items = getProjectList();
    json featured = json::array();
    json categories = json::array({"All"});

    for (const auto &item : items)
    {
        if (item.value("featured", false))
        {
            featured.push_back(item);
        }

        const auto category = item.value("category", "");
        if (!category.empty() && std::find(categories.begin(), categories.end(), category) == categories.end())
        {
            categories.push_back(category);
        }
    }

    return {
        {"site", getSiteProfile()},
        {"profile", getSiteProfile()},
        {"featuredProjects", featured},
        {"featured", featured},
        {"projects", items},
        {"items", items},
        {"categories", categories},
        {"filters", categories}};
}

json Storage_SQL::getProjectDetail(const std::string &slug)
{
    auto row = queryOne(
        projectSelect() +
        "WHERE visibility = 'public' AND slug = ? COLLATE NOCASE LIMIT 1;",
        {slug});

    if (row.empty())
    {
        return json::object();
    }

    auto project = normalizeProject(row);
    project["pages"] = getProjectPages(slug);
    return project;
}

json Storage_SQL::getProjectPages(const std::string &project_slug)
{
    auto rows = queryRows(
        "SELECT pp.slug, pp.title, pp.summary, pp.sort_order AS sortOrder, p.slug AS projectSlug, pp.html_asset_key "
        "FROM project_pages pp "
        "JOIN projects p ON p.id = pp.project_id "
        "WHERE p.slug = ? COLLATE NOCASE AND p.visibility = 'public' AND pp.visibility = 'public' "
        "ORDER BY pp.sort_order ASC, pp.id ASC;",
        {project_slug});

    json pages = json::array();
    for (auto &row : rows)
    {
        pages.push_back(normalizePage(row));
    }
    return pages;
}

json Storage_SQL::getProjectPage(const std::string &project_slug, const std::string &page_slug)
{
    auto row = queryOne(
        "SELECT pp.slug, pp.title, pp.summary, pp.sort_order AS sortOrder, p.slug AS projectSlug, pp.html_asset_key "
        "FROM project_pages pp "
        "JOIN projects p ON p.id = pp.project_id "
        "WHERE p.slug = ? COLLATE NOCASE AND pp.slug = ? COLLATE NOCASE "
        "  AND p.visibility = 'public' AND pp.visibility = 'public' "
        "LIMIT 1;",
        {project_slug, page_slug});

    if (row.empty())
    {
        return json::object();
    }
    return normalizePage(row);
}

std::optional<fs::path> Storage_SQL::resolveAssetPath(const std::string &asset_key)
{
    const fs::path relative_path(asset_key);
    if (!isSafeRelativePath(relative_path))
    {
        return std::nullopt;
    }

    const auto assets_root = absoluteConfiguredPath(Config::AppConfig::Instance()->getAssetsDir());
    const auto file_path = (assets_root / relative_path).lexically_normal();
    std::error_code ec;
    if (!fs::exists(file_path, ec) || !fs::is_regular_file(file_path, ec))
    {
        return std::nullopt;
    }
    return file_path;
}

std::optional<fs::path> Storage_SQL::resolveProjectPagePath(const std::string &project_slug, const std::string &page_slug)
{
    auto row = queryOne(
        "SELECT pp.html_asset_key "
        "FROM project_pages pp "
        "JOIN projects p ON p.id = pp.project_id "
        "WHERE p.slug = ? COLLATE NOCASE AND pp.slug = ? COLLATE NOCASE "
        "  AND p.visibility = 'public' AND pp.visibility = 'public' "
        "LIMIT 1;",
        {project_slug, page_slug});

    if (row.empty())
    {
        return std::nullopt;
    }

    return resolveAssetPath(optionalString(row, "html_asset_key"));
}
}  // namespace Storage
