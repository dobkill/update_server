#include "config/app_config.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace Config {
    namespace {
        std::string findConfigPath()
        {
            namespace fs = std::filesystem;
            const fs::path relative = "config/app.json";
            const fs::path current = fs::current_path();

            for (const auto &base : {current, current.parent_path()})
            {
                const auto candidate = base / relative;
                if (fs::exists(candidate))
                {
                    return fs::absolute(candidate).lexically_normal().string();
                }
            }

            return relative.string();
        }
    }

    std::shared_ptr<AppConfig> AppConfig::s_instance = nullptr;
    std::mutex AppConfig::s_mutex;

    std::shared_ptr<AppConfig> AppConfig::Instance(){
        std::lock_guard<std::mutex> lock(s_mutex);
        if(!s_instance){
            s_instance = std::shared_ptr<AppConfig>(new AppConfig(findConfigPath()));
        }
        return s_instance;
    }

    AppConfig::AppConfig(const std::string &config_path){
        loadConfig(config_path);
    }

    void AppConfig::loadConfig(const std::string &config_path){
        std::ifstream file(config_path);
        if(!file){
            std::cerr << "Error: Failed to open config file: " << config_path << std::endl;
            return;
        }
        
        try{
            json config_json;
            file >> config_json;
            
            if(config_json.contains("sql_file"))
                sql_file = config_json["sql_file"].get<std::string>();
            
            if(config_json.contains("worker_num"))
                worker_num = config_json["worker_num"].get<int>();
            if(config_json.contains("app_name"))
                app_name = config_json["app_name"].get<std::string>();
            if(config_json.contains("database_name"))
                database_name = config_json["database_name"].get<std::string>();
            if(config_json.contains("database_path"))
                database_path = config_json["database_path"].get<std::string>();
            if(config_json.contains("storage_root"))
                storage_root = config_json["storage_root"].get<std::string>();
            if(config_json.contains("upload_task_dir"))
                upload_task_dir = config_json["upload_task_dir"].get<std::string>();
            if(config_json.contains("download_task_dir"))
                download_task_dir = config_json["download_task_dir"].get<std::string>();
            if(config_json.contains("converted_dir"))
                converted_dir = config_json["converted_dir"].get<std::string>();
            if(config_json.contains("failed_dir"))
                failed_dir = config_json["failed_dir"].get<std::string>();
            if(config_json.contains("package_dir"))
                package_dir = config_json["package_dir"].get<std::string>();
            if(config_json.contains("page_staging_dir"))
                page_staging_dir = config_json["page_staging_dir"].get<std::string>();
            if(config_json.contains("page_active_dir"))
                page_active_dir = config_json["page_active_dir"].get<std::string>();
            if(config_json.contains("page_archive_dir"))
                page_archive_dir = config_json["page_archive_dir"].get<std::string>();
            if(config_json.contains("log_dir"))
                log_dir = config_json["log_dir"].get<std::string>();
            if(config_json.contains("convert_script"))
                convert_script = config_json["convert_script"].get<std::string>();
            if(config_json.contains("max_upload_size_bytes"))
                max_upload_size_bytes = config_json["max_upload_size_bytes"].get<int>();
            if(config_json.contains("convert_result_retention_days"))
                convert_result_retention_days = config_json["convert_result_retention_days"].get<int>();
        }catch(const json::exception& e){
            std::cerr << "Error: JSON parsing failed: " << e.what() << std::endl;
        }
        
        file.close();
    }

    std::string AppConfig::getAppName() const{
        return app_name;
    }

    std::string AppConfig::getDatabaseName() const{
        return database_name;
    }

    std::string AppConfig::getDatabasePath() const{
        return database_path;
    }

    std::string AppConfig::getStorageRoot() const{
        return storage_root;
    }

    std::string AppConfig::getUploadTaskDir() const{
        return upload_task_dir;
    }
    
    std::string AppConfig::getSqlFile() const{
        return sql_file;
    }

    std::string AppConfig::getDownloadTaskDir() const{
        return download_task_dir;
    }

    std::string AppConfig::getConvertedDir() const{
        return converted_dir;
    }

    int AppConfig::getWorkerNum() const{
        return worker_num;
    }

    std::string AppConfig::getFailedDir() const{
        return failed_dir;
    }

    std::string AppConfig::getPackageDir() const{
        return package_dir;
    }

    std::string AppConfig::getPageStagingDir() const{
        return page_staging_dir;
    }

    std::string AppConfig::getPageActiveDir() const{
        return page_active_dir;
    }

    std::string AppConfig::getPageArchiveDir() const{
        return page_archive_dir;
    }

    std::string AppConfig::getLogDir() const{
        return log_dir;
    }

    std::string AppConfig::getConvertScript() const{
        return convert_script;
    }

    int AppConfig::getMaxUploadSizeBytes() const{
        return max_upload_size_bytes;
    }

    int AppConfig::getConvertResultRetentionDays() const{
        return convert_result_retention_days;
    }

}
