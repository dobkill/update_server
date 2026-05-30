#include "storage/storage.h"

namespace Storage
{
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

    json Storage_SQL::getProduct(const std::string &product_code)
    {
        return queryOne(
            "SELECT id, code, name, description, icon_path, status, created_at, updated_at "
            "FROM products "
            "WHERE code = ? "
            "LIMIT 1;",
            {product_code});
    }

    json Storage_SQL::getDocument(
        const std::string &product_code,
        const std::string &version,
        const std::string &channel)
    {
        const auto requested_version = version.empty() ? "latest" : version;
        json row;

        if (requested_version == "latest")
        {
            row = queryOne(
                "SELECT "
                "p.code AS product_code, p.name AS product_name, p.description AS product_summary, "
                "r.id AS release_id, r.version, r.release_note, r.data_schema_version, "
                "r.status, r.vue_path, r.published_at, r.created_at, "
                "a.file_path AS download_url, a.file_size, a.platform, a.arch, a.package_type "
                "FROM products p "
                "JOIN release_channels rc ON rc.product_id = p.id "
                "JOIN releases r ON r.id = rc.release_id AND r.product_id = rc.product_id "
                "LEFT JOIN release_assets a ON a.release_id = r.id "
                "  AND a.product_id = p.id "
                "  AND a.platform = rc.platform "
                "  AND a.arch = rc.arch "
                "  AND a.package_type = rc.package_type "
                "WHERE p.code = ? "
                "  AND rc.channel = ? "
                "  AND rc.is_latest = 1 "
                "  AND rc.status = 'active' "
                "LIMIT 1;",
                {product_code, channel.empty() ? "stable" : channel});
        }
        else
        {
            row = queryOne(
                "SELECT "
                "p.code AS product_code, p.name AS product_name, p.description AS product_summary, "
                "r.id AS release_id, r.version, r.release_note, r.data_schema_version, "
                "r.status, r.vue_path, r.published_at, r.created_at, "
                "a.file_path AS download_url, a.file_size, a.platform, a.arch, a.package_type "
                "FROM products p "
                "JOIN releases r ON r.product_id = p.id "
                "LEFT JOIN release_assets a ON a.release_id = r.id AND a.product_id = p.id "
                "WHERE p.code = ? "
                "  AND r.version = ? "
                "LIMIT 1;",
                {product_code, requested_version});
        }

        if (row.empty())
        {
            return json::object();
        }

        const auto safe_channel = channel.empty() ? "stable" : channel;
        const auto resolved_version = jsonString(row, "version");
        const auto release_note = jsonString(row, "release_note");
        const auto title = jsonString(row, "product_name", product_code) + " " + resolved_version;

        json page_data = {
            {"summary", {
                {"title", "版本摘要"},
                {"content", release_note}
            }},
            {"download", {
                {"version", resolved_version},
                {"package_type", jsonString(row, "package_type")},
                {"package_size", row.contains("file_size") && !row["file_size"].is_null() ? std::to_string(row["file_size"].get<long long>()) : ""},
                {"release_note", release_note},
                {"download_text", "下载当前版本"},
                {"download_url", jsonString(row, "download_url")},
                {"platform", jsonString(row, "platform")},
                {"arch", jsonString(row, "arch")},
                {"channel", safe_channel}
            }}
        };

        return {
            {"product_code", product_code},
            {"requested_version", requested_version},
            {"resolved_version", resolved_version},
            {"channel", safe_channel},
            {"title", title},
            {"published_at", jsonString(row, "published_at", jsonString(row, "created_at"))},
            {"release_notes_summary", release_note},
            {"page", {
                {"vue_entry_url", jsonString(row, "vue_path")},
                {"page_data", page_data}
            }}
        };
    }
}
