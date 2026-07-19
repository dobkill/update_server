#pragma once

#include <nlohmann/json.hpp>

#include <memory>
#include <mutex>
#include <string>

namespace Config {
    using json = nlohmann::json;

    class AppConfig {
    public:
        static std::shared_ptr<AppConfig> Instance();

    public:
        std::string getAppName() const;
        std::string getDatabasePath() const;
        std::string getStorageRoot() const;
        std::string getAssetsDir() const;
        std::string getWebDistDir() const;
        std::string getLogDir() const;
        std::string getSqlFile() const;
        int getSqliteBusyTimeoutMs() const;

    private:
        std::string app_name = "personal-project-showcase";
        std::string database_path = "./data/database/showcase.db";
        std::string storage_root = "./data";
        std::string assets_dir = "./data/assets";
        std::string web_dist_dir = "./web/dist";
        std::string log_dir = "./data/logs";
        std::string sql_file = "migrations/001_init_showcase.sql";
        int sqlite_busy_timeout_ms = 5000;

    private:
        static std::shared_ptr<AppConfig> s_instance;
        static std::mutex s_mutex;

        AppConfig(const AppConfig&) = delete;
        AppConfig& operator=(const AppConfig&) = delete;

        AppConfig(const std::string &config_path);
        void loadConfig(const std::string &config_path);
    };
}
