#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <string>

#include "storage/storage.h"

namespace Action
{
    using json = nlohmann::json;

    class ProjectsAction
    {
    public:
        json ListProjects();
        json GetProjectDetail(const std::string &slug);

    private:
        std::shared_ptr<Storage::Storage_SQL> storage = Storage::Storage_SQL::Instance();
    };
}
