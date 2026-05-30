#include "action/CheckUpdateAction.h"

#include <exception>

namespace Action {
    namespace
    {
        std::string jsonString(const json &row, const std::string &key, const std::string &fallback = "")
        {
            if (!row.contains(key) || row[key].is_null())
            {
                return fallback;
            }
            if (row[key].is_string())
            {
                return row[key].get<std::string>();
            }
            return row[key].dump();
        }
    }

    json CheckUpdateAction::CheckUpdate(
        const std::string &product_code,
        const std::string &cur_version,
        const std::string& platform,
        const std::string& arch,
        const std::string& package_type,
        const std::string& cur_data_schema_version)
    {
        if (!storage) {
            return {{"code", 500}, {"message", "storage not initialized"}, {"data", json::object()}};
        }

        json last_release = storage->getLatestRelease(product_code, platform, arch, package_type);
        if(last_release.empty()){
            return {
                {"code", 404},
                {"message", "latest release not found"},
                {"data", {{"product_code", product_code}}}
            };
        }

        json result = json::object();
        result["product_code"] = product_code;
        result["product_id"] = last_release.value("product_id", 0);
        result["current_version"] = cur_version;

        std::string version = jsonString(last_release, "version");
        bool has_update = false;
        try {
            has_update = Tools::canUpgrade(cur_version, version);
        } catch (const std::exception &) {
            has_update = cur_version != version;
        }

        result["has_update"] = has_update;
        if(has_update){
            result["has_update"] = true;
            result["package"]=json::object();
            result["package"]["type"]="full";
            result["package"]["url"]=jsonString(last_release, "file_path");
            result["package"]["file_size"]=last_release.contains("file_size") && !last_release["file_size"].is_null()
                ? last_release["file_size"].get<long long>()
                : 0;
            result["package"]["sha256"]=jsonString(last_release, "sha256", jsonString(last_release, "md5"));
        }

        std::string data_schema_version = jsonString(last_release, "data_schema_version");
        const bool needs_data_upgrade =
            !cur_data_schema_version.empty() &&
            !data_schema_version.empty() &&
            data_schema_version != cur_data_schema_version;

        result["data_upgrade"] = needs_data_upgrade;
        if(needs_data_upgrade){
            result["data_convert"] = {
                {"required", true},
                {"source_data_schema_version", cur_data_schema_version},
                {"target_data_schema_version", data_schema_version},
                {"script_sha256", jsonString(last_release, "script_sha256")}
            };
        }

        result["data_schema_version"]=data_schema_version;
        result["latest_version"]=version;
    
        return {{"code", 0}, {"message", "ok"}, {"data", result}};
    }
}
