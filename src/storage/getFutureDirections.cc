#include "storage/storage.h"

namespace Storage {
    json Storage_SQL::getFutureDirections()
    {
        return queryRows(
            "SELECT title, comment, icon_path "
            "FROM future_directions "
            "WHERE status = 'active' "
            "ORDER BY sort_order ASC, id ASC;");
    }
}
