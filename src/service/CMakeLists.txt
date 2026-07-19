add_library(showcase_service STATIC
    ApiRoutes.cc
)

target_include_directories(showcase_service
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(showcase_service
    PUBLIC
        showcase_storage
    PRIVATE
        Drogon::Drogon
        nlohmann_json::nlohmann_json
)
