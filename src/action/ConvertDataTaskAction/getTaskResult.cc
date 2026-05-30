#include "action/ConvertDataTaskAction.h"

namespace Action {
namespace {
std::string optionalString(const json &row, const std::string &key)
{
    if (!row.contains(key) || row[key].is_null()) {
        return "";
    }
    if (row[key].is_string()) {
        return row[key].get<std::string>();
    }
    return row[key].dump();
}
}

json ConvertDataTaskAction::getTaskResult(const std::string &taskId) {
    Task task;
    bool found = false;

    {
        std::lock_guard<std::mutex> lock(mutex_task);
        auto it = task_map.find(taskId);
        if (it != task_map.end()) {
            task = it->second;
            found = true;
        }
    }

    if (!found) {
        auto db_task = storage ? storage->getTask(taskId) : json::object();
        if (!db_task.empty()) {
            task.task_id = taskId;
            task.status = optionalString(db_task, "status");
            task.input_file_path = optionalString(db_task, "input_file_path");
            task.output_file_path = optionalString(db_task, "output_file_path");
            found = true;
        }
    }

    if (!found) {
        return {
            {"code", 404},
            {"message", "task not found"},
            {"data", {{"task_id", taskId}}}
        };
    }

    if (task.status != "success") {
        return {
            {"code", 1},
            {"message", "task not finished"},
            {"data", {
                {"task_id", task.task_id},
                {"status", task.status}
            }}
        };
    }

    return {
        {"code", 0},
        {"message", "ok"},
        {"data", {
            {"task_id", task.task_id},
            {"status", task.status},
            {"output_file_path", task.output_file_path},
            {"download_url", task.output_file_path}
        }}
    };
}
}
