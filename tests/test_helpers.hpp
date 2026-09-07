#pragma once
#include <cstdint>
#include <filesystem>
#include <string_view>

namespace test_helpers {

namespace fs = std::filesystem;

inline fs::path project_root() {
    return fs::path{__FILE__}.parent_path().parent_path();
}

inline fs::path data_path(std::string_view filename) {
    return project_root() / "data" / filename;
}

inline fs::path test_data_path(std::string_view filename) {
    return project_root() / "tests" / "test_data" / filename;
}

// scripts/generate_data.py создаёт:
// 0, 1, 2, ..., 255, 0, 1, 2, ...
inline std::uint64_t expected_pattern_checksum(std::uintmax_t size) {
    constexpr std::uint64_t cycle_sum =
        255ULL * 256ULL / 2ULL; // 0 + ... + 255 = 32640
    const std::uint64_t n =
        static_cast<std::uint64_t>(size);
    const std::uint64_t full_cycles = n / 256ULL;
    const std::uint64_t remainder = n % 256ULL;
    // 0 + 1 + ... + 
    const std::uint64_t remainder_sum =
        remainder * (remainder - 1ULL) / 2ULL;
    return full_cycles * cycle_sum + remainder_sum;
}

}