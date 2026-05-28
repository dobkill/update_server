#include "app_config.h"
#include <fstream>
#include <iostream>
#include <string>
namespace Config{
    std::shared_ptr<AppConfig> AppConfig::s_instance = nullptr;
    std::mutex AppConfig::s_mutex;

    std::shared_ptr<AppConfig> AppConfig::Instance(){
        const std::string &config_path = "./config/app.json";
        std::lock_guard<std::mutex> lock(s_mutex);
        if(!s_instance){
            s_instance = std::make_shared<AppConfig>(config_path);
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
            
            if(config_json.contains("app_name"))
                app_name = config_json["app_name"].get<std::string>();
            if(config_json.contains("database_name"))
                database_name = config_json["database_name"].get<std::string>();
            if(config_json.contains("database_path"))
                database_path = config_json["database_path"].get<std::string>();
            if(config_json.contains("storage_root"))
                storage_root = config_json["storage_root"].get<std::string>();
            if(config_json.contains("upload_dir"))
                upload_dir = config_json["upload_dir"].get<std::string>();
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

    std::string AppConfig::getAppName(){
        return app_name;
    }

    std::string AppConfig::getDatabaseName(){
        return database_name;
    }

    std::string AppConfig::getDatabasePath(){
        return database_path;
    }

    std::string AppConfig::getStorageRoot(){
        return storage_root;
    }

    std::string AppConfig::getUploadDir(){
        return upload_dir;
    }

    std::string AppConfig::getConvertedDir(){
        return converted_dir;
    }

    std::string AppConfig::getFailedDir(){
        return failed_dir;
    }

    std::string AppConfig::getPackageDir(){
        return package_dir;
    }

    std::string AppConfig::getPageStagingDir(){
        return page_staging_dir;
    }

    std::string AppConfig::getPageActiveDir(){
        return page_active_dir;
    }

    std::string AppConfig::getPageArchiveDir(){
        return page_archive_dir;
    }

    std::string AppConfig::getLogDir(){
        return log_dir;
    }

    std::string AppConfig::getConvertScript(){
        return convert_script;
    }

    int AppConfig::getMaxUploadSizeBytes(){
        return max_upload_size_bytes;
    }

    int AppConfig::getConvertResultRetentionDays(){
        return convert_result_retention_days;
    }

}