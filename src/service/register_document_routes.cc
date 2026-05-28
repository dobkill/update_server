#include "service/ApiRoutes.h"
namespace Router{
    void ApiRoutes::register_document_routes(){
    auto self = shared_from_this();
    drogon::app().registerHandler(
        "/api/v1/products/{product_code}/document",
        [self](
            const drogon::HttpRequestPtr &req,
            std::function<void(const drogon::HttpResponsePtr &)> &&callback,
            const std::string &product_code
        )
        {
            std::string version = req->getParameter("version");
            Json json = self->m_documentAction.GetDocument(product_code, version);
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
            resp->setBody(json.dump());
            callback(resp);
        },
        {drogon::Get}
    );
    }
}
