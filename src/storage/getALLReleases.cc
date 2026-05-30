#include "storage/storage.h"

namespace Storage {
    json Storage_SQL::getALLReleases(const std::string &product_code)
    {
       return queryRows(
           "SELECT "
           "r.id, r.version, r.release_note, r.data_schema_version, r.status, "
           "r.vue_path, r.published_at, r.created_at, r.updated_at "
           "FROM releases r "
           "JOIN products p ON p.id = r.product_id "
           "WHERE p.code = ? "
           "ORDER BY COALESCE(r.published_at, r.created_at) DESC;",
           {product_code});
    }
}
