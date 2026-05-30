#include "storage/storage.h"

namespace Storage {
    json Storage_SQL::getConvertRule(const int product_id, const std::string &source_version, const std::string &target_version)
    {
        return queryOne(
            "SELECT * "
            "FROM convert_rules "
            "WHERE product_id = ? "
            "  AND source_data_schema_version = ? "
            "  AND target_data_schema_version = ? "
            "  AND status = 'active' "
            "LIMIT 1;",
            {std::to_string(product_id), source_version, target_version});
    }
}
