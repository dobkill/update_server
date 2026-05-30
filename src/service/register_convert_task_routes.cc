#include "service/ApiRoutes.h"

#include <filesystem>
#include <fstream>

namespace Router {
    namespace
    {
        Json errorResponse(int code, const std::string &message)
        {
            return {{"code", code}, {"message", message}, {"data", Json::object()}};
        }
    }

    void ApiRoutes::register_convert_task_routes() {
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

                if (from_version.empty() || to_version.empty()) {
                    Json json = errorResponse(400, "from_version and to_version are required");
                    auto resp = drogon::HttpResponse::newHttpResponse();
                    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                    resp->setBody(json.dump());
                    callback(resp);
                    return;
                }

                const std::string file_content(req->body());
                const std::string saved_content = file_content.empty() ? "{}" : file_content;
                const auto upload_dir = std::filesystem::path(Config::AppConfig::Instance()->getUploadTaskDir());
                std::filesystem::create_directories(upload_dir);

                const auto file_name = product_code + "_" + Tools::getTimestampString() + "_" +
                    Tools::getStringMd5(saved_content) + ".json";
                const auto file_save_path = upload_dir / file_name;

                std::ofstream out(file_save_path, std::ios::binary);
                if (!out) {
                    Json json = errorResponse(500, "failed to save upload file");
                    auto resp = drogon::HttpResponse::newHttpResponse();
                    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                    resp->setBody(json.dump());
                    callback(resp);
                    return;
                }
                out << saved_content;
                out.close();

                Json json = self->m_convertTaskAction.createTask(
                    product_code,
                    from_version,
                    to_version,
                    file_save_path.string());
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                resp->setBody(json.dump());
                callback(resp);

            },
            {drogon::Post}
        );
    }
}
