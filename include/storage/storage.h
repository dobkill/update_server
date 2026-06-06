#pragma once

#include <config/app_config.h>
#include <nlohmann/json.hpp>

#include <filesystem>
#include <memory>
#include <mutex>
#include <sqlite3.h>
#include <string>
#include <vector>

namespace Storage {
    using json = nlohmann::json;

    class Storage_SQL {
    public:
        static std::shared_ptr<Storage_SQL> Instance();
        ~Storage_SQL();

        json getProduct(const std::string &product_code);
        json getDocument(const std::string &product_code, const std::string &version, const std::string &channel = "stable");
        json getConvertRule(const int product_id, const std::string &source_version, const std::string &target_version);
        json getALLProducts();
        json getLatestRelease(const std::string &product_code,const std::string& platform,const std::string& arch,const std::string& package_type);
        json getALLReleases(const std::string &product_code);
        json getALLPackages(const std::string &product_code);
        json getRecommendations();
        json getSiteProfile();
        json getFutureDirections();
        json getRecentUpdates(int limit = 10);
        bool addTask(
            const std::string &task_id,
            const int product_id,
            const int convert_rule_id,
            const std::string &status,
            const std::string &source_data_schema_version,
            const std::string &target_data_schema_version,
            const std::string &input_file_path,
            const std::string &input_file_sha256,
            const std::string &output_file_path

        );
        bool updateTaskStatus(const std::string &task_id, const std::string &status,const std::string &output_file_path, const std::string &output_file_sha256);
        json getTask(const std::string &task_id);

    private:
        Storage_SQL() = default;
        Storage_SQL(const Storage_SQL&) = delete;
        Storage_SQL& operator=(const Storage_SQL&) = delete;
        
        static std::shared_ptr<Storage_SQL> s_instance;
        static std::mutex s_mutex;

    private:
        bool init_db();
        bool readSQLFile(const std::string& file_path, std::string& sql_content);
        void execSQL(const std::string& sql);
        bool tableExists(const std::string &table_name);
        bool columnExists(const std::string &table_name, const std::string &column_name);
        bool ensurePortfolioSchema();
        json queryRows(const std::string &sql, const std::vector<std::string> &params = {});
        json queryOne(const std::string &sql, const std::vector<std::string> &params = {});
        bool executeStatement(const std::string &sql, const std::vector<std::string> &params = {});

        sqlite3* db = nullptr;
        std::mutex db_mutex;

    };
}
