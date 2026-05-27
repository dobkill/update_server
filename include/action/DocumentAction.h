#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace Action{
    using json = nlohmann::json;
    class DocumentAction{
    public:
        json GetDocument();
    };
}
