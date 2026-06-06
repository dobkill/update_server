#pragma once
#include <nlohmann/json.hpp>
#include <drogon/HttpResponse.h>
#include <drogon/drogon.h>
#include <action/ProductsAction.h>
#include <action/CheckUpdateAction.h>
#include <action/ConvertDataTaskAction.h>
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

        ApiRoutes(const ApiRoutes &) = delete;
        ApiRoutes &operator=(const ApiRoutes &) = delete;

    private:
        ApiRoutes() = default;
        static std::shared_ptr<ApiRoutes> s_instance;
        static std::mutex s_mutex;

    private:
        Action::ProductsAction m_productsAction;
        Action::CheckUpdateAction m_checkUpdateAction;
        Action::ConvertDataTaskAction m_convertTaskAction;
        Action::DocumentAction m_documentAction;

    private:
        void init_routes();

        void register_portfolio_home_routes(); // 获取首页聚合信息
        void register_products_routes(); // 获取真实产品列表
        void register_document_routes(); // 软件详情页
        void register_release_routes();  // 获取全部产品版本

        void register_check_update_routes(); // 升级检查

        void register_convert_task_routes();        // 数据转换任务
        void register_convert_task_status_routes(); // 数据转换任务状态
        void register_convert_task_result_routes(); // 数据转换任务结果
    };
}
