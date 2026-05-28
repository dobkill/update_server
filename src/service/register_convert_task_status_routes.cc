#include "service/ApiRoutes.h"
namespace Router{
    void ApiRoutes::register_convert_task_status_routes(){
        auto self = shared_from_this();
        drogon::app().registerHandler(
            "/api/v1/products/{product_code}/convert-tasks/{task_id}/status",
            [self](const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback, const std::string &product_code, const std::string &task_id){
                Json json = self->m_convertTaskAction.getTaskStatus(task_id);
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                resp->setBody(json.dump());
                callback(resp);
            },
            {drogon::Get}
        );
    }
}
