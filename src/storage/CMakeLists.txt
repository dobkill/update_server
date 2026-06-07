add_library(update_storage STATIC
    storage.cc
    addTask.cc
    getALLPackages.cc
    getALLProducts.cc
    getALLReleases.cc
    getConvertRule.cc
    getDocument.cc
    getFutureDirections.cc
    getLatestRelease.cc
    getRecentUpdates.cc
    getRecommendations.cc
    getSiteProfile.cc
    getTask.cc
    updateTask.cc
)

target_include_directories(update_storage
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(update_storage
    PUBLIC
        update_config
    PRIVATE
        update_tools
        sqlite3
        nlohmann_json::nlohmann_json
)
