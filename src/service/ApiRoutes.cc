#include "service/ApiRoutes.h"

#include <filesystem>

namespace Router
{
namespace
{
namespace fs = std::filesystem;

std::string normalizeAssetKey(const std::string &raw)
{
    std::string key = raw;
    if (key.empty())
    {
        return key;
    }

    if (key[0] == '/')
    {
        key.erase(0, 1);
    }

    if (!key.empty() && key.back() == '/')
    {
        key.pop_back();
    }

    return key;
}

std::string decodeUrl(const std::string &value)
{
    std::string out;
    out.reserve(value.size());
    for (std::size_t i = 0; i < value.size(); ++i)
    {
        const char ch = value[i];
        if (ch == '%' && i + 2 < value.size())
        {
            const auto hex = [&](char c) {
                if (c >= '0' && c <= '9') return c - '0';
                if (c >= 'a' && c <= 'f') return c - 'a' + 10;
                if (c >= 'A' && c <= 'F') return c - 'A' + 10;
                return -1;
            };
            const int hi = hex(value[i + 1]);
            const int lo = hex(value[i + 2]);
            if (hi >= 0 && lo >= 0)
            {
                out.push_back(static_cast<char>((hi << 4) | lo));
                i += 2;
                continue;
            }
        }
        else if (ch == '+')
        {
            out.push_back(' ');
            continue;
        }
        out.push_back(ch);
    }
    return out;
}
}  // namespace

std::shared_ptr<ApiRoutes> ApiRoutes::s_instance = nullptr;
std::mutex ApiRoutes::s_mutex;

std::shared_ptr<ApiRoutes> ApiRoutes::Instance()
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (!s_instance)
    {
        s_instance = std::shared_ptr<ApiRoutes>(new ApiRoutes());
        s_instance->initRoutes();
    }
    return s_instance;
}

void ApiRoutes::initRoutes()
{
    registerApiRoutes();
    registerFileRoutes();
}

drogon::HttpResponsePtr ApiRoutes::jsonResponse(const Json &body, drogon::HttpStatusCode status)
{
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(status);
    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    resp->setBody(body.dump());
    return resp;
}

drogon::HttpResponsePtr ApiRoutes::success(const Json &data)
{
    return jsonResponse({{"success", true}, {"data", data}});
}

drogon::HttpResponsePtr ApiRoutes::error(
    drogon::HttpStatusCode status,
    const std::string &code,
    const std::string &message,
    const Json &details)
{
    return jsonResponse(
        {{"success", false}, {"error", {{"code", code}, {"message", message}, {"details", details}}}},
        status);
}

void ApiRoutes::registerApiRoutes()
{
    auto self = shared_from_this();

    drogon::app().registerHandler(
        "/api/v1/site",
        [self](const drogon::HttpRequestPtr &req,
               std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
            (void)req;
            if (!self->storage)
            {
                callback(self->error(drogon::k500InternalServerError, "STORAGE_ERROR", "storage not initialized"));
                return;
            }
            callback(self->success(self->storage->getSiteProfile()));
        },
        {drogon::Get});

    drogon::app().registerHandler(
        "/api/v1/home",
        [self](const drogon::HttpRequestPtr &req,
               std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
            (void)req;
            if (!self->storage)
            {
                callback(self->error(drogon::k500InternalServerError, "STORAGE_ERROR", "storage not initialized"));
                return;
            }
            callback(self->success(self->storage->getHomeData()));
        },
        {drogon::Get});

    drogon::app().registerHandler(
        "/api/v1/projects",
        [self](const drogon::HttpRequestPtr &req,
               std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
            (void)req;
            if (!self->storage)
            {
                callback(self->error(drogon::k500InternalServerError, "STORAGE_ERROR", "storage not initialized"));
                return;
            }
            callback(self->success({{"items", self->storage->getProjectList()}}));
        },
        {drogon::Get});

    drogon::app().registerHandler(
        "/api/v1/projects/{slug}",
        [self](const drogon::HttpRequestPtr &req,
               std::function<void(const drogon::HttpResponsePtr &)> &&callback,
               const std::string &slug) {
            (void)req;
            if (!self->storage)
            {
                callback(self->error(drogon::k500InternalServerError, "STORAGE_ERROR", "storage not initialized"));
                return;
            }
            auto project = self->storage->getProjectDetail(slug);
            if (project.empty())
            {
                callback(self->error(
                    drogon::k404NotFound,
                    "PROJECT_NOT_FOUND",
                    "project not found",
                    {{"slug", slug}}));
                return;
            }
            callback(self->success({{"project", project}, {"profile", self->storage->getSiteProfile()}}));
        },
        {drogon::Get});

    drogon::app().registerHandler(
        "/api/v1/projects/{slug}/pages",
        [self](const drogon::HttpRequestPtr &req,
               std::function<void(const drogon::HttpResponsePtr &)> &&callback,
               const std::string &slug) {
            (void)req;
            if (!self->storage)
            {
                callback(self->error(drogon::k500InternalServerError, "STORAGE_ERROR", "storage not initialized"));
                return;
            }
            callback(self->success({{"items", self->storage->getProjectPages(slug)}}));
        },
        {drogon::Get});
}

void ApiRoutes::registerFileRoutes()
{
    auto self = shared_from_this();

    drogon::app().registerHandlerViaRegex(
        "^/assets/(.+)$",
        [self](const drogon::HttpRequestPtr &req,
               std::function<void(const drogon::HttpResponsePtr &)> &&callback,
               const std::string &asset_path) {
            if (!self->storage)
            {
                callback(self->error(drogon::k500InternalServerError, "STORAGE_ERROR", "storage not initialized"));
                return;
            }

            const auto key = normalizeAssetKey(decodeUrl(asset_path));
            const auto resolved = self->storage->resolveAssetPath(key);
            if (!resolved)
            {
                callback(self->error(
                    drogon::k404NotFound,
                    "ASSET_NOT_FOUND",
                    "asset not found",
                    {{"asset_key", key}}));
                return;
            }

            callback(drogon::HttpResponse::newFileResponse(resolved->string(), "", drogon::CT_NONE, "", req));
        },
        {drogon::Get});

    drogon::app().registerHandler(
        "/projects/{slug}/pages/{page_slug}",
        [self](const drogon::HttpRequestPtr &req,
               std::function<void(const drogon::HttpResponsePtr &)> &&callback,
               const std::string &slug,
               const std::string &page_slug) {
            (void)req;
            if (!self->storage)
            {
                callback(self->error(drogon::k500InternalServerError, "STORAGE_ERROR", "storage not initialized"));
                return;
            }

            const auto resolved = self->storage->resolveProjectPagePath(slug, page_slug);
            if (!resolved)
            {
                callback(self->error(
                    drogon::k404NotFound,
                    "PROJECT_PAGE_NOT_FOUND",
                    "project page not found",
                    {{"slug", slug}, {"page_slug", page_slug}}));
                return;
            }

            callback(drogon::HttpResponse::newFileResponse(
                resolved->string(), "", drogon::CT_TEXT_HTML, "", req));
        },
        {drogon::Get});
}
}  // namespace Router
