
#pragma once
#include <nlohmann/json.hpp>
#include <string>
namespace Action{
    using json = nlohmann::json;
    class ListReleasesAction{
    public:
        json ListReleases();
    };
}
