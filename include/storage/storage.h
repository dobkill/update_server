#pragma once
#include <config/app_config.h>
#include "migrations/001_init.sql"
#include <filesystem>
#include <nlohmann/json.hpp>
#include <memory>
#include <sqlite3.h>
#include <mutex>
namespace Storage{
    using json = nlohmann::json;
    class Storage_SQL{
    public:
        static std::shared_ptr<Storage_SQL> Instance();
        json getDocument(const std::string &product_code, const std::string &version);
        json getALLProducts();
        json getALLReleases(const std::string &product_code);
        json getALLPackages(const std::string &product_code);
        json createTask(const std::string &task_id,
            const int product_id,
            const int convert_rule_id,
            const int source_release_id,
            const int target_release_id,
            const std::string &source_data_schema_version,
            const std::string &target_data_schema_version,
            const std::string &status,
            const std::string &input_file_path,
            const std::string &output_file_path,
            const std::string &input_file_sha256,
            const std::string &output_file_sha256
        );
        json updateTask(const std::string &task_id, const std::string &status, const int retry_count,const std::string &output_file_path, const std::string &output_file_sha256);
        json getTask(const std::string &task_id);
    private:
        Storage_SQL() = default;
        ~Storage_SQL();
        Storage_SQL(const Storage_SQL&) = delete;
        Storage_SQL& operator=(const Storage_SQL&) = delete;
        
        static std::shared_ptr<Storage_SQL> s_instance;
        static std::mutex s_mutex;
    private:
        void init_db();
        bool readSQLFile(const std::string& file_path, std::string& sql_content);
        void execSQL(const std::string& sql);
        sqlite3* db = nullptr;

    };
}