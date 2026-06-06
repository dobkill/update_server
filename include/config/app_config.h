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
        std::string getDatabaseName() const;
        std::string getDatabasePath() const;
        std::string getStorageRoot() const;
        std::string getUploadTaskDir() const;
        std::string getDownloadTaskDir() const;
        std::string getConvertedDir() const;
        std::string getFailedDir() const;
        std::string getPackageDir() const;
        std::string getPageStagingDir() const;
        std::string getPageActiveDir() const;
        std::string getPageArchiveDir() const;
        std::string getLogDir() const;
        std::string getConvertScript() const;
        std::string getSqlFile() const;
        
        int getMaxUploadSizeBytes() const;
        int getConvertResultRetentionDays() const;
        int getWorkerNum() const;

    private:
        std::string app_name = "software-update-platform";
        std::string database_name = "default";
        std::string database_path = "./data/database/update_platform.db";
        std::string storage_root = "./data";
        std::string upload_task_dir = "./data/uploads/convert";
        std::string download_task_dir = "./data/downloads/convert";
        std::string converted_dir = "./data/converted";
        std::string failed_dir = "./data/failed";
        std::string package_dir = "./data/packages";
        std::string page_staging_dir = "./data/releases/staging";
        std::string page_active_dir = "./data/releases/active";
        std::string page_archive_dir = "./data/releases/archive";
        std::string log_dir = "./data/logs";
        std::string convert_script = "./python/convert/convert_data.py";
        std::string sql_file = "migrations/002.init.sql";
        int max_upload_size_bytes = 52428800;
        int convert_result_retention_days = 7;
        int worker_num = 1;

    private:
        static std::shared_ptr<AppConfig> s_instance;
        static std::mutex s_mutex;

        AppConfig(const AppConfig&) = delete;
        AppConfig& operator=(const AppConfig&) = delete;

        AppConfig(const std::string &config_path);
        void loadConfig(const std::string &config_path);
    };
}
