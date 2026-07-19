#pragma once

#include <config/app_config.h>
#include <nlohmann/json.hpp>
#include <sqlite3.h>

#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

namespace Storage
{
using json = nlohmann::json;

class Storage_SQL
{
public:
    static std::shared_ptr<Storage_SQL> Instance();
    ~Storage_SQL();

    json getSiteProfile();
    json getHomeData();
    json getProjectList();
    json getProjectDetail(const std::string &slug);
    json getProjectPages(const std::string &project_slug);
    json getProjectPage(const std::string &project_slug, const std::string &page_slug);
    std::optional<std::filesystem::path> resolveAssetPath(const std::string &asset_key);
    std::optional<std::filesystem::path> resolveProjectPagePath(const std::string &project_slug, const std::string &page_slug);

private:
    Storage_SQL() = default;
    Storage_SQL(const Storage_SQL &) = delete;
    Storage_SQL &operator=(const Storage_SQL &) = delete;

    static std::shared_ptr<Storage_SQL> s_instance;
    static std::mutex s_mutex;

    bool initDb();
    bool runMigrations();
    bool readSqlFile(const std::string &file_path, std::string &sql_content);
    bool executeSqlScript(const std::string &sql);
    bool executeStatement(const std::string &sql, const std::vector<std::string> &params = {});
    json queryRows(const std::string &sql, const std::vector<std::string> &params = {});
    json queryOne(const std::string &sql, const std::vector<std::string> &params = {});
    bool tableExists(const std::string &table_name);

    sqlite3 *db = nullptr;
    std::mutex db_mutex;
};
}  // namespace Storage
