add_executable(software_update_platform
    main.cpp
)

target_link_libraries(software_update_platform
    PRIVATE
        showcase_service
        showcase_storage
        showcase_config
        Drogon::Drogon
        spdlog::spdlog
)
