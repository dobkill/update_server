#include "service/ApiRoutes.h"
namespace Router{
    void ApiRoutes::register_check_update_routes(){
        auto self = shared_from_this();
        drogon::app().registerHandler(
            "/api/v1/products/{product_code}/check-update/{version}",
            [self](const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback, const std::string &product_code, const std::string &version){

            Json json =self->m_checkUpdateAction.CheckUpdate(product_code, version);
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
            resp->setBody(json.dump());

            callback(resp);
            },
            {drogon::Get}
        );
    }
}
