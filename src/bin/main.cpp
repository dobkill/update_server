#include <drogon/HttpResponse.h>
#include <drogon/drogon.h>
#include <array>
#include <config/app_config.h>
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
    app.setDocumentRoot("./data");
    app.setDefaultHandler(
        [webIndex](const drogon::HttpRequestPtr &req,
                   std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
                callback(drogon::HttpResponse::newFileResponse(webIndex.string(), "", drogon::CT_NONE, "", req));
        });

    app.run();
    return 0;
}
