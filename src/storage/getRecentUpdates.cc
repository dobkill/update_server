#include "storage/storage.h"

namespace Storage {
    json Storage_SQL::getRecentUpdates(int limit)
    {
        if (limit <= 0)
        {
            limit = 10;
        }

        return queryRows(
            "SELECT "
            "p.code AS product_code, "
            "p.name AS name, "
            "r.version AS version, "
            "COALESCE(r.published_at, r.created_at) AS published_at, "
            "substr(COALESCE(r.published_at, r.created_at), 1, 10) || ' · ' || p.name || ' ' || r.version || ' 发布' AS title, "
            "COALESCE(r.release_note, p.description, '') AS description "
            "FROM release_channels rc "
            "JOIN releases r ON r.id = rc.release_id AND r.product_id = rc.product_id "
            "JOIN products p ON p.id = rc.product_id "
            "WHERE rc.channel = 'stable' "
            "  AND rc.status = 'active' "
            "  AND r.status = 'active' "
            "  AND p.status = 'active' "
            "GROUP BY r.id "
            "ORDER BY COALESCE(r.published_at, r.created_at) DESC "
            "LIMIT ?;",
            {std::to_string(limit)});
    }
}
