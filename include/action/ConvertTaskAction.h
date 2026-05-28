#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <mutex>
#include <map>
namespace Action{
    using json = nlohmann::json;
    class ConvertTaskAction{
    public:
        json createTask(const std::string &product_code, const std::string &from_version, const std::string &to_version);
        json getTaskStatus(const std::string &taskId);
        json getTaskResult(const std::string &taskId);
    private:
        std::mutex mutex_task;
        std::map<std::string, json> tasks;
    };
}
