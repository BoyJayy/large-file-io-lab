#pragma once
#include <filesystem>

namespace largeio {
    bool is_regular_file(const std::string& path);
}