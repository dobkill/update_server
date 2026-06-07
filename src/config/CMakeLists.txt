add_library(update_config STATIC
    app_config.cc
)

target_include_directories(update_config
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(update_config
    PRIVATE
        nlohmann_json::nlohmann_json
)
