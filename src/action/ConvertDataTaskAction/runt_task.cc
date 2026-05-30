#include "action/ConvertDataTaskAction.h"

#include <cstdlib>
#include <exception>
#include <filesystem>

namespace Action {
namespace {
std::string shellQuote(const std::string &value)
{
    std::string quoted = "'";
    for (char ch : value) {
        if (ch == '\'') {
            quoted += "'\\''";
        } else {
            quoted += ch;
        }
    }
    quoted += "'";
    return quoted;
}
}

ConvertDataTaskAction::ConvertDataTaskAction() {
    if (worker_num <= 0) {
        worker_num = 1;
    }

    for (int i = 0; i < worker_num; ++i) {
        workers.emplace_back(&ConvertDataTaskAction::consume_task, this);
    }
}

ConvertDataTaskAction::~ConvertDataTaskAction() {
    {
        std::lock_guard<std::mutex> lock(mutex_task);
        stopping = true;
    }

    cv_task.notify_all();

    for (auto &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}
bool ConvertDataTaskAction::run_task(const Task &one) {
    {
        std::lock_guard<std::mutex> lock(mutex_task);

        tasks.push(one);
        task_map[one.task_id] = one;
        task_map[one.task_id].status = "pending";
    }

    cv_task.notify_one();
    return true;
}
void ConvertDataTaskAction::consume_task() {
    while (true) {
        Task task;

        {
            std::unique_lock<std::mutex> lock(mutex_task);

            cv_task.wait(lock, [this]() {
                return stopping || !tasks.empty();
            });

            if (stopping && tasks.empty()) {
                return;
            }

            task = tasks.front();
            tasks.pop();

            task.status = "running";
            task_map[task.task_id] = task;
        }

        // 注意：真正执行任务时不要持有 mutex
        bool ok = execute_task(task);

        task.status = ok ? "success" : "failed";

        call_back(task);
    }
}
bool ConvertDataTaskAction::execute_task(const Task &one) {
    std::filesystem::create_directories(std::filesystem::path(one.output_file_path).parent_path());

    std::string cmd = "python3 " + shellQuote(one.script_path) +
        " --input " + shellQuote(one.input_file_path) +
        " --output " + shellQuote(one.output_file_path) +
        " --source-version " + shellQuote(one.source_data_schema_version) +
        " --target-version " + shellQuote(one.target_data_schema_version);

    int ret = std::system(cmd.c_str());

    return ret == 0;
}
bool ConvertDataTaskAction::call_back(const Task &one) {
    {
        std::lock_guard<std::mutex> lock(mutex_task);
        task_map[one.task_id] = one;
    }
    std::string output_sha256;
    if (one.status == "success") {
        try {
            output_sha256 = Tools::getFileSha256(one.output_file_path);
        } catch (const std::exception &) {
            output_sha256.clear();
        }
    }

    return storage ? storage->updateTaskStatus(one.task_id, one.status, one.output_file_path, output_sha256) : false;
}
}
