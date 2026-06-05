#include <drogon/HttpResponse.h>
#include <drogon/drogon.h>
#include <array>
#include <config/app_config.h>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <service/ApiRoutes.h>
#include <storage/storage.h>
#include <string_view>

namespace fs = std::filesystem;
namespace
{
using json = nlohmann::json;

fs::path findPath(std::string_view relativePath)
{
    const fs::path target(relativePath);
    const auto current = fs::current_path();

    for (const auto &base : std::array<fs::path, 2>{current, current.parent_path()})
    {
        const auto candidate = base / target;
        if (fs::exists(candidate))
        {
            return fs::absolute(candidate).lexically_normal();
        }
    }

    return fs::absolute(target).lexically_normal();
}


}  // namespace

int main()
{
    const auto configPath = findPath("config/drogon_config.json");
    const auto webRoot = findPath("web/dist");
    const auto webIndex = webRoot / "index.html";
    const auto databasePath = findPath("data/database/update_platform.db");
    const auto dataRoot = findPath("data");
    const auto convertScriptPath = findPath("python/convert/convert_data.py");

    auto &app = drogon::app();
    Config::AppConfig::Instance();

    if (fs::exists(configPath))
    {
        app.loadConfigFile(configPath.string());
    }

    if (fs::exists(webRoot))
    {
        app.setDocumentRoot(webRoot.string());
        app.setHomePage("index.html");
    }

    Router::ApiRoutes::Instance();

    app.setDefaultHandler(
        [webIndex, dataRoot](const drogon::HttpRequestPtr &req,
                   std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
            const auto requestPath = req->path();
            if (requestPath.rfind("/data/", 0) == 0)
            {
                auto relativePath = fs::path(requestPath.substr(std::string_view("/data/").size()));
                if (relativePath.is_absolute())
                {
                    auto resp = drogon::HttpResponse::newHttpResponse();
                    resp->setStatusCode(drogon::k403Forbidden);
                    callback(resp);
                    return;
                }

                for (const auto &part : relativePath)
                {
                    if (part == "..")
                    {
                        auto resp = drogon::HttpResponse::newHttpResponse();
                        resp->setStatusCode(drogon::k403Forbidden);
                        callback(resp);
                        return;
                    }
                }

                const auto filePath = (dataRoot / relativePath).lexically_normal();
                if (!fs::exists(filePath) || !fs::is_regular_file(filePath))
                {
                    auto resp = drogon::HttpResponse::newHttpResponse();
                    resp->setStatusCode(drogon::k404NotFound);
                    callback(resp);
                    return;
                }

                callback(drogon::HttpResponse::newFileResponse(filePath.string(), "", drogon::CT_NONE, "", req));
                return;
            }

            if (!fs::exists(webIndex))
            {
                json body = {
                    {"code", 404},
                    {"message", "web/dist/index.html not found"},
                    {"data", json::object()}
                };
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(drogon::k404NotFound);
                resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                resp->setBody(body.dump());
                callback(resp);
                return;
            }

            (void)req;
                callback(drogon::HttpResponse::newFileResponse(webIndex.string(), "", drogon::CT_NONE, "", req));
        });

    app.run();
    return 0;
}
