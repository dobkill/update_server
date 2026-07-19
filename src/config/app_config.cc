#include "config/app_config.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace Config
{
namespace
{
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

template <typename T>
void assignIfPresent(const json &config, const char *key, T &target)
{
    if (config.contains(key) && !config.at(key).is_null())
    {
        target = config.at(key).get<T>();
    }
}
}  // namespace

std::shared_ptr<AppConfig> AppConfig::s_instance = nullptr;
std::mutex AppConfig::s_mutex;

std::shared_ptr<AppConfig> AppConfig::Instance()
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (!s_instance)
    {
        s_instance = std::shared_ptr<AppConfig>(new AppConfig(findConfigPath()));
    }
    return s_instance;
}

AppConfig::AppConfig(const std::string &config_path)
{
    loadConfig(config_path);
}

void AppConfig::loadConfig(const std::string &config_path)
{
    std::ifstream file(config_path);
    if (!file)
    {
        std::cerr << "Config file not found, using defaults: " << config_path << std::endl;
        return;
    }

    try
    {
        json config_json;
        file >> config_json;

        assignIfPresent(config_json, "app_name", app_name);
        assignIfPresent(config_json, "database_path", database_path);
        assignIfPresent(config_json, "storage_root", storage_root);
        assignIfPresent(config_json, "assets_dir", assets_dir);
        assignIfPresent(config_json, "web_dist_dir", web_dist_dir);
        assignIfPresent(config_json, "log_dir", log_dir);
        assignIfPresent(config_json, "sql_file", sql_file);
        assignIfPresent(config_json, "sqlite_busy_timeout_ms", sqlite_busy_timeout_ms);
    }
    catch (const json::exception &e)
    {
        std::cerr << "Error: JSON parsing failed: " << e.what() << std::endl;
    }
}

std::string AppConfig::getAppName() const
{
    return app_name;
}

std::string AppConfig::getDatabasePath() const
{
    return database_path;
}

std::string AppConfig::getStorageRoot() const
{
    return storage_root;
}

std::string AppConfig::getAssetsDir() const
{
    return assets_dir;
}

std::string AppConfig::getWebDistDir() const
{
    return web_dist_dir;
}

std::string AppConfig::getLogDir() const
{
    return log_dir;
}

std::string AppConfig::getSqlFile() const
{
    return sql_file;
}

int AppConfig::getSqliteBusyTimeoutMs() const
{
    return sqlite_busy_timeout_ms;
}
}  // namespace Config
