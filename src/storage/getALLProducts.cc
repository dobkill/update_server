#include "storage/storage.h"

namespace Storage {
    json Storage_SQL::getALLProducts()
    {
        return queryRows(
            "SELECT "
            "p.code AS product_code, "
            "p.name AS name, "
            "COALESCE(p.description, '') AS summary, "
            "p.icon_path AS cover_image_url, "
            "(SELECT r.version "
            "   FROM release_channels rc "
            "   JOIN releases r ON r.id = rc.release_id AND r.product_id = rc.product_id "
            "  WHERE rc.product_id = p.id "
            "    AND rc.channel = 'stable' "
            "    AND rc.is_latest = 1 "
            "    AND rc.status = 'active' "
            "  LIMIT 1) AS latest_version, "
            "p.updated_at AS updated_at "
            "FROM products p "
            "WHERE p.status = 'active' "
            "ORDER BY p.created_at DESC;");
    }
}
