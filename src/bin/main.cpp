#include <drogon/HttpResponse.h>
#include <drogon/drogon.h>
#include <array>
#include <config/app_config.h>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <service/ApiRoutes.h>
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
        [webIndex](const drogon::HttpRequestPtr &req,
                   std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
            const auto requestPath = req->path();

            if (requestPath.rfind("/api/", 0) == 0 ||
                requestPath.rfind("/assets/", 0) == 0)
            {
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(drogon::k404NotFound);
                resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                resp->setBody(json{
                    {"success", false},
                    {"error", {{"code", "NOT_FOUND"}, {"message", "route not found"}, {"details", {{"path", requestPath}}}}}
                }.dump());
                callback(resp);
                return;
            }

            if (!fs::exists(webIndex))
            {
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(drogon::k404NotFound);
                resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                resp->setBody(json{
                    {"success", false},
                    {"error", {{"code", "WEB_NOT_FOUND"}, {"message", "web/dist/index.html not found"}}}
                }.dump());
                callback(resp);
                return;
            }

            callback(drogon::HttpResponse::newFileResponse(webIndex.string(), "", drogon::CT_NONE, "", req));
        });

    app.run();
    return 0;
}
