#pragma once

#include <openssl/evp.h>

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace Tools {
    std::string getFileMd5(const std::string &file_path);
    std::string getStringMd5(const std::string &str);
    std::string getFileSha256(const std::string &file_path);
    std::string getTimestampString() ;
    bool canUpgrade(const std::string &cur_version, const std::string &target_version);
}
