#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <memory>
#include <mutex>
namespace Config{
    using json = nlohmann::json;
    class AppConfig{
    public:
        static std::shared_ptr<AppConfig> Instance();

    public:
        std::string getAppName();
        std::string getDatabaseName();
        std::string getDatabasePath();
        std::string getStorageRoot();
        std::string getUploadDir();
        std::string getConvertedDir();
        std::string getFailedDir();
        std::string getPackageDir();
        std::string getPageStagingDir();
        std::string getPageActiveDir();
        std::string getPageArchiveDir();
        std::string getLogDir();
        std::string getConvertScript();
        int getMaxUploadSizeBytes();
        int getConvertResultRetentionDays();
    private:
      std::string app_name;
      std::string database_name;
      std::string database_path;
      std::string storage_root;
      std::string upload_dir;
      std::string converted_dir;
      std::string failed_dir;
      std::string package_dir;
      std::string page_staging_dir;
      std::string page_active_dir;
      std::string page_archive_dir;
      std::string log_dir;
      std::string convert_script;
      int max_upload_size_bytes;
      int convert_result_retention_days;

    private:
        static std::shared_ptr<AppConfig> s_instance;
        static std::mutex s_mutex;

AppConfig(const AppConfig&) = delete;
AppConfig& operator=(const AppConfig&) = delete;

        AppConfig(const std::string &config_path);
        void loadConfig(const std::string &config_path);

    };

}