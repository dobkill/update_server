#pragma once
#include <nlohmann/json.hpp>

#include "config/app_config.h"
#include "storage/storage.h"
#include "tools/tools.h"

#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

namespace Action {
    using json = nlohmann::json;

    struct Task {
        std::string task_id; // 时间戳 + file_hash
        std::string status;
        std::string source_data_schema_version;
        std::string target_data_schema_version;
        std::string input_file_path;
        std::string output_file_path;
        std::string script_path;
    };
    
    class ConvertDataTaskAction {
    public:
        ConvertDataTaskAction();
        ~ConvertDataTaskAction();

        json createTask(
            const std::string &product_code,
            const std::string &from_version,
            const std::string &to_version,
            const std::string &file_path);

        json getTaskStatus(const std::string &taskId);
        json getTaskResult(const std::string &taskId);

    private:
        std::mutex mutex_task;
        std::condition_variable cv_task;

        std::queue<Task> tasks;
        std::map<std::string, Task> task_map;

        std::shared_ptr<Storage::Storage_SQL> storage = Storage::Storage_SQL::Instance();

    private:
        std::vector<std::thread> workers;
        bool stopping = false;

        int worker_num = Config::AppConfig::Instance()->getWorkerNum();

        bool run_task(const Task &one);      // 入队
        bool call_back(const Task &one);     // 执行完成后更新状态
        void consume_task();                 // 消费线程函数
        bool execute_task(const Task &one);  // 真正执行脚本
    };
}
