#include "storage/storage.h"

namespace Storage {
    bool Storage_SQL::updateTaskStatus(const std::string &task_id, const std::string &status,const std::string &output_file_path, const std::string &output_file_sha256)
    {
        return executeStatement(
            "UPDATE convert_tasks "
            "SET status = ?, output_file_path = ?, output_file_sha256 = ?, updated_at = CURRENT_TIMESTAMP "
            "WHERE task_id = ?;",
            {status, output_file_path, output_file_sha256, task_id});
    }
}
