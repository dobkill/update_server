#include "action/ProjectsAction.h"

namespace Action
{
    json ProjectsAction::GetProjectDetail(const std::string &slug)
    {
        if (!storage)
        {
            return {{"code", 500}, {"message", "storage not initialized"}, {"data", json::object()}};
        }

        auto project = storage->getPortfolioProjectDetail(slug);
        if (project.empty())
        {
            return {
                {"code", 404},
                {"message", "project not found"},
                {"data", {{"slug", slug}}}
            };
        }

        return {
            {"code", 0},
            {"message", "ok"},
            {"data", {
                {"profile", storage->getSiteProfile()},
                {"project", project}
            }}
        };
    }
}
