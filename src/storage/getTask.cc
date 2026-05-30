#include "storage/storage.h"

namespace Storage {
    json Storage_SQL::getTask(const std::string &task_id)
    {
        return queryOne(
            "SELECT * FROM convert_tasks WHERE task_id = ? LIMIT 1;",
            {task_id});
    }
}
