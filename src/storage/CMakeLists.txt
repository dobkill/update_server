add_library(showcase_storage STATIC
    storage.cc
)

target_include_directories(showcase_storage
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(showcase_storage
    PUBLIC
        showcase_config
    PRIVATE
        sqlite3
        nlohmann_json::nlohmann_json
)
