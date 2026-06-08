#include "action/ProjectsAction.h"

namespace Action
{
    json ProjectsAction::ListProjects()
    {
        if (!storage)
        {
            return {{"code", 500}, {"message", "storage not initialized"}, {"data", json::object()}};
        }

        return {
            {"code", 0},
            {"message", "ok"},
            {"data", {
                {"profile", storage->getSiteProfile()},
                {"featured", storage->getFeaturedPortfolioProjects()},
                {"items", storage->getPortfolioProjectList()},
                {"filters", json::array({"All", "Web App", "AI", "Automation", "Tool", "Data"})}
            }}
        };
    }
}
