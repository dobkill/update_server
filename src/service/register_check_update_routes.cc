#include "service/ApiRoutes.h"

namespace Router {
    void ApiRoutes::register_check_update_routes() {
        auto self = shared_from_this();
        drogon::app().registerHandler(
            "/api/v1/products/{product_code}/check-update/{version}",
            [self](
                const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                const std::string &product_code,
                const std::string &version)
            {
                const auto platform = req->getParameter("platform").empty()
                    ? "windows"
                    : req->getParameter("platform");
                const auto arch = req->getParameter("arch").empty()
                    ? "x64"
                    : req->getParameter("arch");
                const auto package_type = req->getParameter("package_type").empty()
                    ? "portable"
                    : req->getParameter("package_type");
                const auto data_schema_version = req->getParameter("data_schema_version");

                Json json = self->m_checkUpdateAction.CheckUpdate(
                    product_code,
                    version,
                    platform,
                    arch,
                    package_type,
                    data_schema_version);

                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                resp->setBody(json.dump());
                callback(resp);
            },
            {drogon::Get}
        );
    }
}
