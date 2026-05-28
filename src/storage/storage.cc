#include "storage.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

namespace Storage
{

    Storage_SQL::~Storage_SQL()
    {
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

        if (need_init)
        {
            std::string sql_content;
            if (!readSQLFile("migrations/002.init.sql", sql_content))
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

        return true;
    }

    std::shared_ptr<Storage_SQL> Storage_SQL::Instance()
    {
        std::lock_guard<std::mutex> lock(s_mutex);

        if (!s_instance)
        {
            auto instance = std::make_shared<Storage_SQL>();

            if (!instance->init_db())
            {
                std::cerr << "Failed to initialize database" << std::endl;
                return nullptr;
            }

            s_instance = instance;
        }

        return s_instance;
    }

}