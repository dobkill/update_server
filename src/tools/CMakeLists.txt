add_library(update_tools STATIC
    canupgrade.cc
    md5.cc
    sha256.cc
    TimestampString.cc
)

target_include_directories(update_tools
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(update_tools
    PRIVATE
        OpenSSL::Crypto
)
