add_executable(software_update_platform
    main.cpp
)

target_link_libraries(software_update_platform
    PRIVATE
        update_service
        update_storage
        update_config
        Drogon::Drogon
        spdlog::spdlog
        OpenSSL::Crypto
)
