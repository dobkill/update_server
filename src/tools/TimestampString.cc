#include "tools/tools.h"
namespace Tools
{
    std::string getTimestampString()
    {
        auto now = std::chrono::system_clock::now();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch());
        return std::to_string(milliseconds.count());
    }
}
