#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace Action{
    using json = nlohmann::json;
    class DocumentAction{
    public:
        json GetDocument(const std::string &product_code, const std::string &version);
    };
}
