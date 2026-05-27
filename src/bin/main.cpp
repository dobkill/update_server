#include <drogon/HttpResponse.h>
#include <drogon/drogon.h>
#include <array>
#include <filesystem>
#include <string_view>
#include <nlohmann/json.hpp>

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

bool shouldServeWebIndex(const drogon::HttpRequestPtr &req)
{
    if (req->method() != drogon::Get && req->method() != drogon::Head)
    {
        return false;
    }

    const auto &path = req->path();
    if (path.starts_with("/api/") || path == "/healthz" || path == "/readyz" || path.starts_with("/public/"))
    {
        return false;
    }

    return path.find('.') == std::string::npos;
}

Json::Value buildHealthPayload()
{
    Json::Value payload;
    payload["status"] = "ok";
    return payload;
}

Json::Value buildReadyPayload(bool databaseReady, bool dataReady, bool taskRunnerReady)
{
    Json::Value payload;
    payload["status"] = databaseReady && dataReady && taskRunnerReady ? "ready" : "not_ready";
    payload["checks"]["database"] = databaseReady ? "ok" : "missing";
    payload["checks"]["data"] = dataReady ? "ok" : "missing";
    payload["checks"]["task_runner"] = taskRunnerReady ? "ok" : "missing";
    return payload;
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

    if (fs::exists(configPath))
    {
        app.loadConfigFile(configPath.string());
    }

    if (fs::exists(webRoot))
    {
        app.setDocumentRoot(webRoot.string());
        app.setHomePage("index.html");
    }

    app.registerHandler(
        "/healthz",
        [](const drogon::HttpRequestPtr &,
           std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
            callback(drogon::HttpResponse::newHttpJsonResponse(buildHealthPayload()));
        },
        {drogon::Get});

    app.registerHandler(
        "/readyz",
        [databasePath, dataRoot, convertScriptPath](
            const drogon::HttpRequestPtr &,
            std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
            const bool databaseReady = fs::exists(databasePath) && fs::is_regular_file(databasePath);
            const bool dataReady = fs::exists(dataRoot) && fs::is_directory(dataRoot);
            const bool taskRunnerReady = fs::exists(convertScriptPath) && fs::is_regular_file(convertScriptPath);

            auto response = drogon::HttpResponse::newHttpJsonResponse(
                buildReadyPayload(databaseReady, dataReady, taskRunnerReady));
            if (!(databaseReady && dataReady && taskRunnerReady))
            {
                response->setStatusCode(drogon::k503ServiceUnavailable);
            }

            callback(response);
        },
        {drogon::Get});

    app.setDefaultHandler(
        [webIndex](const drogon::HttpRequestPtr &req,
                   std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
            if (shouldServeWebIndex(req))
            {
                callback(drogon::HttpResponse::newFileResponse(webIndex.string(), "", drogon::CT_NONE, "", req));
                return;
            }
            callback(drogon::HttpResponse::newNotFoundResponse(req));
        });

    app.run();
    return 0;
}
