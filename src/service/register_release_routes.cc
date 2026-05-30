#include "service/ApiRoutes.h"
namespace Router {   

void ApiRoutes::register_release_routes()
{
    auto self = shared_from_this();

    drogon::app().registerHandler(
        "/api/v1/products/{product_code}/releases",
        [self](
            const drogon::HttpRequestPtr &req,
            std::function<void(const drogon::HttpResponsePtr &)> &&callback,
            const std::string &product_code
        )
        {
            (void)req;
            auto json = self->m_documentAction.GetListReleases(product_code);
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
            resp->setBody(json.dump());
            callback(resp);
        },
        {drogon::Get}
    );
}

}
