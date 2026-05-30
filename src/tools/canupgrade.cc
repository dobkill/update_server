#include "tools/tools.h"
#include <string>
#include <string_view>
#include <charconv>
#include <cstdint>
#include <stdexcept>

namespace Tools{
static std::uint64_t readVersionPart(std::string_view version, std::size_t& pos) {
    std::size_t start = pos;

    while (pos < version.size() && version[pos] != '.') {
        ++pos;
    }

    if (start == pos) {
        throw std::invalid_argument("invalid version format");
    }

    std::uint64_t value = 0;

    const char* begin = version.data() + start;
    const char* end = version.data() + pos;

    auto result = std::from_chars(begin, end, value);

    if (result.ec != std::errc{} || result.ptr != end) {
        throw std::invalid_argument("invalid version number");
    }

    if (pos < version.size() && version[pos] == '.') {
        ++pos;
    }

    return value;
}

// 返回：
// -1: cur_version < target_version
//  0: cur_version == target_version
//  1: cur_version > target_version
static int compareVersion(std::string_view cur_version, std::string_view target_version) {
    std::size_t i = 0;
    std::size_t j = 0;

    while (i < cur_version.size() || j < target_version.size()) {
        std::uint64_t cur_part = 0;
        std::uint64_t target_part = 0;

        if (i < cur_version.size()) {
            cur_part = readVersionPart(cur_version, i);
        }

        if (j < target_version.size()) {
            target_part = readVersionPart(target_version, j);
        }

        if (cur_part < target_part) return -1;
        if (cur_part > target_part) return 1;
    }

    return 0;
}

bool canUpgrade(const std::string& cur_version, const std::string& target_version) {
    return compareVersion(cur_version, target_version) < 0;
}
}
