#pragma once
#include <nlohmann/json.hpp>

#include <memory>
#include <string>

#include "storage/storage.h"

namespace Action {
    using json = nlohmann::json;

    class DocumentAction {
    public:
        json GetDocument(const std::string &product_code, const std::string &version, const std::string &channel = "stable");
        json GetListReleases(const std::string &product_code);

    private:
        std::shared_ptr<Storage::Storage_SQL> storage = Storage::Storage_SQL::Instance();
    };
}
