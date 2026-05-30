#include "service/ApiRoutes.h"

namespace Router
{
std::shared_ptr<ApiRoutes> ApiRoutes::s_instance = nullptr;
std::mutex ApiRoutes::s_mutex;
void ApiRoutes::init_routes()
{
    register_products_routes();
    register_check_update_routes();
    register_convert_task_routes();
    register_convert_task_status_routes();
    register_convert_task_result_routes();
    register_document_routes();
    register_release_routes();
}
std::shared_ptr<ApiRoutes> ApiRoutes::Instance()
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (!s_instance)
    {
        s_instance = std::shared_ptr<ApiRoutes>(new ApiRoutes());
        s_instance->init_routes();
    }
    return s_instance;
}

}
