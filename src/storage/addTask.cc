#include "storage/storage.h"

namespace Storage {
    bool Storage_SQL::addTask(
        const std::string &task_id,
        const int product_id,
        const int convert_rule_id,
        const std::string &status,
        const std::string &source_data_schema_version,
        const std::string &target_data_schema_version,
        const std::string &input_file_path,
        const std::string &input_file_sha256,
        const std::string &output_file_path)
    {
        return executeStatement(
            "INSERT INTO convert_tasks ("
            "task_id, product_id, convert_rule_id, source_data_schema_version, "
            "target_data_schema_version, status, input_file_path, input_file_sha256, "
            "output_file_path"
            ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);",
            {
                task_id,
                std::to_string(product_id),
                std::to_string(convert_rule_id),
                source_data_schema_version,
                target_data_schema_version,
                status,
                input_file_path,
                input_file_sha256,
                output_file_path
            });
    }
}
