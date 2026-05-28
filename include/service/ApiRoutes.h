#pragma once
#include <nlohmann/json.hpp>
#include <drogon/HttpResponse.h>
#include <drogon/drogon.h>
#include <action/ProductsAction.h>
#include <action/CheckUpdateAction.h>
#include <action/ConvertTaskAction.h>
#include <action/DownloadAction.h>
#include <action/DocumentAction.h>

#include <memory>
#include <mutex>

namespace Router
{
using Json = nlohmann::json;
class ApiRoutes : public std::enable_shared_from_this<ApiRoutes>
{
public:
    static std::shared_ptr<ApiRoutes> Instance();


    ApiRoutes(const ApiRoutes&) = delete;
    ApiRoutes& operator=(const ApiRoutes&) = delete;

private:
    ApiRoutes() = default;

    static std::shared_ptr<ApiRoutes> s_instance;
    static std::mutex s_mutex;

private:
    Action::ProductsAction m_productsAction;
    Action::CheckUpdateAction m_checkUpdateAction;
    Action::ConvertTaskAction m_convertTaskAction;
    Action::DownloadAction m_downloadAction;
    Action::DocumentAction m_documentAction;

private:
    void init_routes();
    void register_products_routes();
    void register_check_update_routes();
    void register_convert_task_routes();
    void register_convert_task_status_routes();
    void register_convert_task_result_routes();
    void register_download_routes();
    void register_document_routes();
};
}