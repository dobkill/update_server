cmake_minimum_required(VERSION 3.20)

project(software_update_platform VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SHOWCASE_BUILD_WEB "Build web assets with external Node tooling" OFF)

find_package(Drogon CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_subdirectory(src)
