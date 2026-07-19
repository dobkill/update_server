#pragma once

#include <drogon/HttpResponse.h>
#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <storage/storage.h>

#include <memory>
#include <mutex>
#include <string>

namespace Router
{
using Json = nlohmann::json;

class ApiRoutes : public std::enable_shared_from_this<ApiRoutes>
{
public:
    static std::shared_ptr<ApiRoutes> Instance();

    ApiRoutes(const ApiRoutes &) = delete;
    ApiRoutes &operator=(const ApiRoutes &) = delete;

private:
    ApiRoutes() = default;

    static std::shared_ptr<ApiRoutes> s_instance;
    static std::mutex s_mutex;

    std::shared_ptr<Storage::Storage_SQL> storage = Storage::Storage_SQL::Instance();

    void initRoutes();
    void registerApiRoutes();
    void registerFileRoutes();

    drogon::HttpResponsePtr jsonResponse(const Json &body, drogon::HttpStatusCode status = drogon::k200OK);
    drogon::HttpResponsePtr success(const Json &data);
    drogon::HttpResponsePtr error(
        drogon::HttpStatusCode status,
        const std::string &code,
        const std::string &message,
        const Json &details = Json::object());
};
}  // namespace Router
