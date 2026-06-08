#include "service/ApiRoutes.h"

namespace Router
{
    void ApiRoutes::register_projects_routes()
    {
        auto self = shared_from_this();

        drogon::app().registerHandler(
            "/api/v1/projects",
            [self](
                const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                (void)req;
                auto json = self->m_projectsAction.ListProjects();
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                resp->setBody(json.dump());
                callback(resp);
            },
            {drogon::Get});

        drogon::app().registerHandler(
            "/api/v1/projects/{slug}",
            [self](
                const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                const std::string &slug)
            {
                (void)req;
                auto json = self->m_projectsAction.GetProjectDetail(slug);
                auto resp = drogon::HttpResponse::newHttpResponse();
                if (json.value("code", 0) == 404)
                {
                    resp->setStatusCode(drogon::k404NotFound);
                }
                resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                resp->setBody(json.dump());
                callback(resp);
            },
            {drogon::Get});
    }
}
