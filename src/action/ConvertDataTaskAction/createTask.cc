#include "action/ConvertDataTaskAction.h"

#include <exception>
#include <filesystem>

namespace Action {
json ConvertDataTaskAction::createTask(
    const std::string &product_code,
    const std::string &from_version,
    const std::string &to_version,
    const std::string &file_path)
{
    if (!storage) {
        return {{"code", 500}, {"message", "storage not initialized"}, {"data", json::object()}};
    }

    auto product = storage->getProduct(product_code);
    if (product.empty()) {
        return {
            {"code", 404},
            {"message", "product not found"},
            {"data", {{"product_code", product_code}}}
        };
    }

    const int product_id = product.value("id", 0);
    Task task;

    std::string file_hash;
    try {
        file_hash = Tools::getFileSha256(file_path);
    } catch (const std::exception &e) {
        return {
            {"code", 400},
            {"message", e.what()},
            {"data", {{"input_file_path", file_path}}}
        };
    }

    const std::string timestamp = Tools::getTimestampString();
    task.task_id = product_code + "_" + from_version + "_" + to_version + "_" + timestamp + "_" + file_hash;
    task.status = "pending";
    task.source_data_schema_version = from_version;
    task.target_data_schema_version = to_version;
    task.input_file_path = file_path;

    const auto output_dir = std::filesystem::path(Config::AppConfig::Instance()->getConvertedDir());
    std::filesystem::create_directories(output_dir);
    task.output_file_path = (output_dir / (task.task_id + ".json")).string();

    json rule_json = storage->getConvertRule(product_id, from_version, to_version);
    if (rule_json.empty())
    {
        return {
            {"code", 404},
            {"message", "convert rule not found"},
            {"data", {
                {"product_code", product_code},
                {"source_data_schema_version", from_version},
                {"target_data_schema_version", to_version}
            }}
        };
    }

    task.script_path = rule_json.value("script_path", Config::AppConfig::Instance()->getConvertScript());
    const bool added = storage->addTask(
        task.task_id,
        product_id,
        rule_json.value("id", 0),
        task.status,
        from_version,
        to_version,
        task.input_file_path,
        file_hash,
        task.output_file_path);

    if (!added) {
        return {
            {"code", 500},
            {"message", "failed to create convert task"},
            {"data", {{"task_id", task.task_id}}}
        };
    }

    run_task(task);
    return {
        {"code", 0},
        {"message", "ok"},
        {"data", {
            {"task_id", task.task_id},
            {"status", task.status},
            {"cost_time", rule_json.value("cost_time", 5)}
        }}
    };
}
}
