add_library(showcase_config STATIC
    app_config.cc
)

target_include_directories(showcase_config
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(showcase_config
    PRIVATE
        nlohmann_json::nlohmann_json
)
