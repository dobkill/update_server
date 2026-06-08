add_library(update_action STATIC
    CheckUpdataAction/checkupdate.cc
    ConvertDataTaskAction/createTask.cc
    ConvertDataTaskAction/getTaskResult.cc
    ConvertDataTaskAction/getTaskStatus.cc
    ConvertDataTaskAction/runt_task.cc
    DocumentAction/getDocumnet.cc
    DocumentAction/getReleases.cc
    ProjectsAction/GetProjectDetail.cc
    ProjectsAction/ListProjects.cc
)

target_include_directories(update_action
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(update_action
    PUBLIC
        update_storage
    PRIVATE
        update_tools
        update_config
        nlohmann_json::nlohmann_json
)
