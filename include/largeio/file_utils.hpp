#pragma once
#include <filesystem>
namespace fs =  std::filesystem;

namespace largeio {
    bool is_regular_file(const fs::path& path);
}