#include "storage/storage.h"

namespace Storage {
    json Storage_SQL::getLatestRelease(
        const std::string &product_code,
        const std::string& platform,
        const std::string& arch,
        const std::string& package_type)
    {
        return queryOne(
            "SELECT "
            "p.id AS product_id, p.code AS product_code, "
            "r.id AS release_id, r.version, r.data_schema_version, "
            "a.file_path, a.file_size, a.sha256, a.md5, a.status, "
            "cr.script_path AS convert_script_path, cr.script_sha256 AS script_sha256 "
            "FROM products p "
            "JOIN release_channels rc ON rc.product_id = p.id "
            "JOIN releases r ON r.id = rc.release_id AND r.product_id = rc.product_id "
            "LEFT JOIN release_assets a ON a.release_id = r.id "
            "  AND a.product_id = p.id "
            "  AND a.platform = rc.platform "
            "  AND a.arch = rc.arch "
            "  AND a.package_type = rc.package_type "
            "LEFT JOIN convert_rules cr ON cr.product_id = p.id "
            "  AND cr.target_data_schema_version = r.data_schema_version "
            "  AND cr.status = 'active' "
            "WHERE p.code = ? COLLATE NOCASE "
            "  AND rc.channel = 'stable' "
            "  AND rc.platform = ? "
            "  AND rc.arch = ? "
            "  AND rc.package_type = ? "
            "  AND rc.is_latest = 1 "
            "  AND rc.status = 'active' "
            "LIMIT 1;",
            {product_code, platform, arch, package_type});
    }
}
