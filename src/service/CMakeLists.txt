add_library(update_service STATIC
    ApiRoutes.cc
    register_check_update_routes.cc
    register_convert_task_result_routes.cc
    register_convert_task_routes.cc
    register_convert_task_status_routes.cc
    register_document_routes.cc
    register_projects_routes.cc
    register_release_routes.cc
)

target_include_directories(update_service
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(update_service
    PUBLIC
        update_action
    PRIVATE
        Drogon::Drogon
        nlohmann_json::nlohmann_json
)
