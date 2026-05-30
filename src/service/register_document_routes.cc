#include "service/ApiRoutes.h"

namespace Router {
    void ApiRoutes::register_document_routes() {
        auto self = shared_from_this();

        drogon::app().registerHandler(
            "/api/v1/products/{product_code}/document",
            [self](
                const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                const std::string &product_code)
            {
                std::string version = req->getParameter("version");
                if (version.empty()) {
                    version = "latest";
                }

                std::string channel = req->getParameter("channel");
                if (channel.empty()) {
                    channel = "stable";
                }

                Json json = self->m_documentAction.GetDocument(product_code, version, channel);
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                resp->setBody(json.dump());
                callback(resp);
            },
            {drogon::Get}
        );

        drogon::app().registerHandler(
            "/api/v1/products/{product_code}/Document",
            [self](
                const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                const std::string &product_code)
            {
                std::string version = req->getParameter("version");
                if (version.empty()) {
                    version = "latest";
                }

                std::string channel = req->getParameter("channel");
                if (channel.empty()) {
                    channel = "stable";
                }

                Json json = self->m_documentAction.GetDocument(product_code, version, channel);
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                resp->setBody(json.dump());
                callback(resp);
            },
            {drogon::Get}
        );
    }
}
