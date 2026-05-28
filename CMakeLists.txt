cmake_minimum_required(VERSION 3.20)

project(software_update_platform VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(UPDATE_PLATFORM_BUILD_WEB "Build web assets with external Node tooling" OFF)

find_package(Drogon CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(sqlite3 CONFIG REQUIRED)

file(GLOB_RECURSE action_sources "src/action/*.cc")

file(GLOB_RECURSE service_sources "src/service/*.cc")

file(GLOB_RECURSE storage_sources "src/storage/*.cc")

add_executable(software_update_platform
    src/bin/main.cpp
    ${action_sources}

    ${service_sources}


    src/config/app_config.cc
    src/storage/file_store.cc

    ${storage_sources}
)
include_directories(include) 



target_link_libraries(software_update_platform
    PRIVATE
        Drogon::Drogon
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        sqlite3::sqlite3
)

enable_testing()
