#pragma once
#include <nlohmann/json.hpp>
namespace Action{
    using json = nlohmann::json;
    class CheckUpdateAction{
    public:
        json CheckUpdate(const std::string &product_code, const std::string &version);
    };    
    
}