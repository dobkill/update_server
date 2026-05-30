#pragma once
#include <nlohmann/json.hpp>

#include <memory>
#include <string>

#include "storage/storage.h"
#include "tools/tools.h"

namespace Action {
    using json = nlohmann::json;

    class CheckUpdateAction {
    public:
        json CheckUpdate(
            const std::string &product_code,
            const std::string &cur_version,
            const std::string& platform,
            const std::string& arch,
            const std::string& package_type,
            const std::string& cur_data_schema_version = "");

    private:
        std::shared_ptr<Storage::Storage_SQL> storage = Storage::Storage_SQL::Instance();
    };    
    
}
