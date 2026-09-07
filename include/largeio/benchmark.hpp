#pragma once
#include <cstdint>
#include <filesystem>
#include <string>

namespace largeio {

struct BenchmarkResult {
    std::string method;
    std::uintmax_t file_size;
    std::size_t block_size;
    std::uint64_t checksum;
    double seconds;
    double throughput_mib_s; // будем в мб в сек следовательно делим на 2^20=1024*1024
};

void print_benchmark_result(const BenchmarkResult& result);

//тестовый бенч
BenchmarkResult benchmark_ifstream(const std::filesystem::path& path, std::size_t block_size);
BenchmarkResult benchmark_fd(const std::filesystem::path& path, std::size_t block_size);
BenchmarkResult benchmark_mmap(const std::filesystem::path& path);

}