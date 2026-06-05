#include "storage/storage.h"

namespace Storage {
    json Storage_SQL::getALLPackages(const std::string &product_code)
    {
        return queryRows(
            "SELECT "
            "a.id, a.release_id, r.version, a.platform, a.arch, a.package_type, "
            "a.file_path, a.file_size, a.sha256, a.md5, a.status, a.created_at "
            "FROM release_assets a "
            "JOIN releases r ON r.id = a.release_id AND r.product_id = a.product_id "
            "JOIN products p ON p.id = a.product_id "
            "WHERE p.code = ? COLLATE NOCASE "
            "ORDER BY r.created_at DESC, a.created_at DESC;",
            {product_code});
    }
}
