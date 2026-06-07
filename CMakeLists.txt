cmake_minimum_required(VERSION 3.20)

project(software_update_platform VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(UPDATE_PLATFORM_BUILD_WEB "Build web assets with external Node toolting" OFF)

# ── 全局外部依赖 ──
find_package(Drogon CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

# ── 源码子目录 ──
add_subdirectory(src)
