#include "action/ConvertDataTaskAction.h"

namespace Action
{
  json ConvertDataTaskAction::getTaskStatus(const std::string &taskId) {
    {
        std::lock_guard<std::mutex> lock(mutex_task);
        auto it = task_map.find(taskId);
        if (it != task_map.end()) {
            return {
                {"code", 0},
                {"message", "ok"},
                {"data", {
                    {"task_id", it->second.task_id},
                    {"status", it->second.status}
                }}
            };
        }
    }

    auto task = storage ? storage->getTask(taskId) : json::object();
    if (task.empty()) {
        return {
            {"code", 404},
            {"message", "task not found"},
            {"data", {{"task_id", taskId}}}
        };
    }

    return {
        {"code", 0},
        {"message", "ok"},
        {"data", {
            {"task_id", taskId},
            {"status", task.value("status", "")}
        }}
    };
}
}
