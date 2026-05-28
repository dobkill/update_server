#include "service/ApiRoutes.h"
namespace Router{
    void ApiRoutes::register_convert_task_routes(){
        auto self = shared_from_this();
        drogon::app().registerHandler(
            "/api/v1/products/{product_code}/convert-tasks",
            [self](
                const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                const std::string &product_code
            )
            {
                std::string from_version = req->getParameter("from_version");
                std::string to_version = req->getParameter("to_version");
                Json json = self->m_convertTaskAction.createTask(product_code, from_version, to_version);
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                resp->setBody(json.dump());
                callback(resp);

            },
            {drogon::Post}
        );
    }
}
