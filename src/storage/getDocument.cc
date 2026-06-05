#include "storage/storage.h"

#include <array>
#include <filesystem>
#include <fstream>
#include <string_view>

namespace Storage
{
    namespace fs = std::filesystem;

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

        fs::path findPath(std::string_view relativePath)
        {
            const fs::path target(relativePath);
            const auto current = fs::current_path();

            for (const auto &base : std::array<fs::path, 2>{current, current.parent_path()})
            {
                const auto candidate = base / target;
                if (fs::exists(candidate))
                {
                    return fs::absolute(candidate).lexically_normal();
                }
            }

            return fs::absolute(target).lexically_normal();
        }

        json loadReleasePageData(const std::string &product_code, const std::string &version)
        {
            if (product_code.empty() || version.empty())
            {
                return json::object();
            }

            const auto page_manifest = findPath(
                "data/releases/" + product_code + "/" + version + "/vue/page.json");
            if (!fs::exists(page_manifest) || !fs::is_regular_file(page_manifest))
            {
                return json::object();
            }

            std::ifstream input(page_manifest);
            if (!input)
            {
                return json::object();
            }

            try
            {
                json payload = json::parse(input);
                if (payload.contains("page_data") && payload["page_data"].is_object())
                {
                    return payload["page_data"];
                }
                return payload.is_object() ? payload : json::object();
            }
            catch (const json::parse_error &)
            {
                return json::object();
            }
        }
    }

    json Storage_SQL::getProduct(const std::string &product_code)
    {
        return queryOne(
            "SELECT id, code, name, description, icon_path, status, created_at, updated_at "
            "FROM products "
            "WHERE code = ? COLLATE NOCASE "
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
                "WHERE p.code = ? COLLATE NOCASE "
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
                "WHERE p.code = ? COLLATE NOCASE "
                "  AND r.version = ? "
                "LIMIT 1;",
                {product_code, requested_version});
        }

        if (row.empty())
        {
            return json::object();
        }

        const auto safe_channel = channel.empty() ? "stable" : channel;
        const auto resolved_product_code = jsonString(row, "product_code", product_code);
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
        const auto manifest_page_data = loadReleasePageData(resolved_product_code, resolved_version);
        if (!manifest_page_data.empty())
        {
            page_data = manifest_page_data;
            if (!page_data.contains("download"))
            {
                page_data["download"] = {
                    {"version", resolved_version},
                    {"package_type", jsonString(row, "package_type")},
                    {"package_size", row.contains("file_size") && !row["file_size"].is_null() ? std::to_string(row["file_size"].get<long long>()) : ""},
                    {"release_note", release_note},
                    {"download_text", "下载当前版本"},
                    {"download_url", jsonString(row, "download_url")},
                    {"platform", jsonString(row, "platform")},
                    {"arch", jsonString(row, "arch")},
                    {"channel", safe_channel}
                };
            }
        }

        return {
            {"product_code", resolved_product_code},
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
