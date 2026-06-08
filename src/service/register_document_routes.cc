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
                const std::string request_path = req->path();
                const std::string suffix = "/document";
                if (request_path.size() < suffix.size() ||
                    request_path.compare(request_path.size() - suffix.size(), suffix.size(), suffix) != 0)
                {
                    Json body = {
                        {"code", 404},
                        {"message", "api route not found"},
                        {"data", {{"path", request_path}}}
                    };
                    auto resp = drogon::HttpResponse::newHttpResponse();
                    resp->setStatusCode(drogon::k404NotFound);
                    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                    resp->setBody(body.dump());
                    callback(resp);
                    return;
                }

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
