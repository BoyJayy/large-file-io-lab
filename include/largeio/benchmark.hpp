#pragma once
#include <cstdint>
#include <filesystem>
#include <string>

namespace largeio {

enum class BenchmarkMethod {
    Ifstream,
    PosixRead,
    Mmap
};

struct BenchmarkResult {
    std::string method;
    std::uintmax_t file_size;
    std::size_t block_size;
    std::uint64_t checksum;
    double seconds;
    double throughput_mib_s; // будем в мб в сек следовательно делим на 2^20=1024*1024
};
struct BenchmarkSummary {
    std::string file;
    std::string method;
    std::uintmax_t file_size;
    std::size_t block_size;
    std::uint64_t checksum;
    std::vector<double> times;
    double min_seconds;
    double median_seconds;
    double max_seconds;
    double median_throughput_mib_s;
};

void print_benchmark_result(const BenchmarkResult& result);

//тестовый бенч
BenchmarkResult benchmark_ifstream(const std::filesystem::path& path, std::size_t block_size);
BenchmarkResult benchmark_fd(const std::filesystem::path& path, std::size_t block_size);
BenchmarkResult benchmark_mmap(const std::filesystem::path& path);
std::vector<BenchmarkSummary> run_benchmark_suite(const std::filesystem::path& path, std::size_t repeats = 7);
void print_benchmark_result(const BenchmarkResult& result);
void print_benchmark_summary(const BenchmarkSummary& summary);
void save_benchmark_summary_csv(const std::filesystem::path& csv_path, const std::vector<BenchmarkSummary>& summaries);
void save_benchmark_raw_csv(const std::filesystem::path& csv_path, const std::vector<BenchmarkSummary>& summaries);


}