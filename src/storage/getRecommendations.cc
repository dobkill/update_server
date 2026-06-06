#include "storage/storage.h"

namespace Storage {
    json Storage_SQL::getRecommendations()
    {
        return queryRows(
            "SELECT "
            "p.code AS product_code, "
            "p.name AS name, "
            "COALESCE(p.description, '') AS summary, "
            "COALESCE(p.icon_path, '') AS cover_image_url, "
            "COALESCE(p.github_url, '') AS github_url, "
            "(SELECT r.version "
            "   FROM release_channels rc "
            "   JOIN releases r ON r.id = rc.release_id AND r.product_id = rc.product_id "
            "  WHERE rc.product_id = p.id "
            "    AND rc.channel = 'stable' "
            "    AND rc.is_latest = 1 "
            "    AND rc.status = 'active' "
            "    AND r.status = 'active' "
            "  ORDER BY COALESCE(r.published_at, r.created_at) DESC "
            "  LIMIT 1) AS latest_version, "
            "(SELECT COALESCE(r.published_at, r.created_at) "
            "   FROM release_channels rc "
            "   JOIN releases r ON r.id = rc.release_id AND r.product_id = rc.product_id "
            "  WHERE rc.product_id = p.id "
            "    AND rc.channel = 'stable' "
            "    AND rc.is_latest = 1 "
            "    AND rc.status = 'active' "
            "    AND r.status = 'active' "
            "  ORDER BY COALESCE(r.published_at, r.created_at) DESC "
            "  LIMIT 1) AS published_at, "
            "'Obsidian 插件' AS tag, "
            "'已发布' AS status_label "
            "FROM recommendations rec "
            "JOIN products p ON p.id = rec.product_id "
            "WHERE rec.status = 'active' "
            "  AND p.status = 'active' "
            "ORDER BY rec.sort_order ASC, rec.id ASC;");
    }
}
