#include "service/ApiRoutes.h"

namespace Router {

void ApiRoutes::register_download_routes() {
    auto self = shared_from_this();

    drogon::app().registerHandler(
        "/api/v1/products/{product_code}/releases/{version}/download",
        [self](
            const drogon::HttpRequestPtr &req,
            std::function<void(const drogon::HttpResponsePtr &)> &&callback,
            const std::string &product_code,
            const std::string &version_to
        )
        {
            //curl "http://127.0.0.1:8080/api/v1/products/abc/releases/1.2.0/download?type=incremental&current_version=1.0.0"
            std::string mode = req->getParameter("mode");
            std::string from_version = req->getParameter("from_version");
            // product_code 和 version 就是 URL 中的路径参数
            Json json = self->m_downloadAction.download(product_code, mode, from_version, version_to);
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
            resp->setBody(json.dump());
            callback(resp);
        },
        {drogon::Get}
    );
}

}
