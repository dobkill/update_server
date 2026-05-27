#pragma once
#include <nlohmann/json.hpp>
namespace Action{
    using json = nlohmann::json;
    class CheckUpdateAction{
    public:
        json buildPayload();
    };

    
}