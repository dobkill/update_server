#pragma once
#include <nlohmann/json.hpp>
#include <mutex>
#include <string>
#include <map>
namespace Action{
    using json = nlohmann::json;
    class ConvertTaskAction{
    public:
        json createTask(const json &task);
        json getTaskStatus(const std::string &taskId);
        json getTaskResult(const std::string &taskId);
    private:
        std::mutex mutex_task;
        std::map<std::string, json> tasks;
    };
}