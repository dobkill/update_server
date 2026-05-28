#pragma once
#include <nlohmann/json.hpp>
namespace Action{
    using json = nlohmann::json;
    class DownloadAction{
    public:
        json download(const std::string &product_code, const std::string &mode, const std::string &from_version, const std::string &version_to);
    };
}
