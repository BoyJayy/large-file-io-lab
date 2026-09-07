#include <largeio/benchmark.hpp>
#include <largeio/file_read.hpp>
#include <largeio/file_utils.hpp>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>

namespace largeio {

namespace {

struct BenchmarkConfig {
    BenchmarkMethod method;
    std::size_t block_size;
};

double calculate_throughput(std::uintmax_t file_size, double seconds) {
    if (seconds <= 0.0) return 0.0;
    double file_size_mib = static_cast<double>(file_size) / (1024.0 * 1024.0);
    return file_size_mib / seconds;
}

double median(std::vector<double> values) {
    if (values.empty()) return 0.0;
    std::sort(values.begin(), values.end());
    std::size_t n = values.size();
    if (n % 2 == 1) return values[n / 2];
    return (values[n / 2 - 1] + values[n / 2]) / 2.0;
}

std::string method_name(BenchmarkMethod method) {
    if (method == BenchmarkMethod::Ifstream) return "ifstream";
    if (method == BenchmarkMethod::PosixRead) return "posix_read";
    return "mmap";
}

BenchmarkResult run_config(const std::filesystem::path& path, const BenchmarkConfig& config) {
    if (config.method == BenchmarkMethod::Ifstream)
        return benchmark_ifstream(path, config.block_size);
    if (config.method == BenchmarkMethod::PosixRead)
        return benchmark_fd(path, config.block_size);
    return benchmark_mmap(path);
}
}

BenchmarkResult benchmark_ifstream(const std::filesystem::path& path, std::size_t block_size) {
    BenchmarkResult result;
    result.method = "ifstream";
    result.file_size = std::filesystem::file_size(path);
    result.block_size = block_size;
    auto start = std::chrono::steady_clock::now();
    result.checksum = checksum_ifstream(path, block_size);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    result.seconds = elapsed.count();
    result.throughput_mib_s = calculate_throughput(result.file_size, result.seconds);
    return result;
}

BenchmarkResult benchmark_fd(const std::filesystem::path& path, std::size_t block_size) {
    BenchmarkResult result;
    result.method = "posix_read";
    result.file_size = std::filesystem::file_size(path);
    result.block_size = block_size;
    auto start = std::chrono::steady_clock::now();
    result.checksum = checksum_fd(path, block_size);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    result.seconds = elapsed.count();
    result.throughput_mib_s = calculate_throughput(result.file_size, result.seconds);
    return result;
}

BenchmarkResult benchmark_mmap(const std::filesystem::path& path) {
    BenchmarkResult result;
    result.method = "mmap";
    result.file_size = std::filesystem::file_size(path);
    result.block_size = 0;
    auto start = std::chrono::steady_clock::now();
    result.checksum = checksum_mmap(path);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    result.seconds = elapsed.count();
    result.throughput_mib_s = calculate_throughput(result.file_size, result.seconds);
    return result;
}

std::vector<BenchmarkSummary> run_benchmark_suite(const std::filesystem::path& path, std::size_t repeats) {
    if (!largeio::is_regular_file(path))
        throw std::runtime_error("benchmark path is not a regular file");
    if (repeats == 0)
        throw std::invalid_argument("repeats must be greater than 0");
    const std::size_t KiB = 1024;
    const std::size_t MiB = 1024 * KiB;
    std::vector<BenchmarkConfig> configs = {
        {BenchmarkMethod::Ifstream, 4 * KiB},
        {BenchmarkMethod::Ifstream, 64 * KiB},
        {BenchmarkMethod::Ifstream, 1 * MiB},
        {BenchmarkMethod::Ifstream, 4 * MiB},
        {BenchmarkMethod::PosixRead, 4 * KiB},
        {BenchmarkMethod::PosixRead, 64 * KiB},
        {BenchmarkMethod::PosixRead, 1 * MiB},
        {BenchmarkMethod::PosixRead, 4 * MiB},
        {BenchmarkMethod::Mmap, 0}
    };
    std::uintmax_t file_size = std::filesystem::file_size(path);
    std::cout << "Warm-up: " << path << '\n';
    std::uint64_t expected_checksum = checksum_fd(path, 1 * MiB);
    std::vector<BenchmarkSummary> summaries;
    summaries.reserve(configs.size());
    for (const auto& config : configs) {
        BenchmarkSummary summary;
        summary.file = path.string();
        summary.method = method_name(config.method);
        summary.file_size = file_size;
        summary.block_size = config.block_size;
        summary.checksum = expected_checksum;

        summary.min_seconds = 0.0;
        summary.median_seconds = 0.0;
        summary.max_seconds = 0.0;
        summary.median_throughput_mib_s = 0.0;

        summaries.push_back(summary);
    }
    std::vector<std::size_t> order(configs.size());
    std::iota(order.begin(), order.end(), 0);
    std::mt19937 rng(0xC0FFEE);
    for (std::size_t round = 0; round < repeats; round++) {
        std::shuffle(order.begin(), order.end(), rng);
        std::cout << "Round " << round + 1 << "/" << repeats << '\n';
        for (std::size_t index : order) {
            BenchmarkResult result = run_config(path, configs[index]);
            if (result.checksum != expected_checksum)
                throw std::runtime_error("checksum mismatch during benchmark");
            summaries[index].times.push_back(result.seconds);
            std::cout << "  " << result.method;
            if (result.block_size != 0)
                std::cout << " block=" << result.block_size;
            std::cout << " time=" << result.seconds << " s";
            std::cout << " throughput=" << result.throughput_mib_s << " MiB/s\n";
        }
    }
    for (auto& summary : summaries) {
        auto minmax = std::minmax_element(summary.times.begin(), summary.times.end());
        summary.min_seconds = *minmax.first;
        summary.max_seconds = *minmax.second;
        summary.median_seconds = median(summary.times);
        summary.median_throughput_mib_s = calculate_throughput(summary.file_size, summary.median_seconds);
    }
    return summaries;
}

void print_benchmark_result(const BenchmarkResult& result) {
    std::cout << "Method: " << result.method << '\n';
    std::cout << "File size: " << result.file_size << " bytes\n";
    if (result.block_size == 0)
        std::cout << "Block size: N/A\n";
    else
        std::cout << "Block size: " << result.block_size << " bytes\n";
    std::cout << "Checksum: " << result.checksum << '\n';
    std::cout << "Time: " << result.seconds << " s\n";
    std::cout << "Throughput: " << result.throughput_mib_s << " MiB/s\n";
}

void print_benchmark_summary(const BenchmarkSummary& summary) {
    std::cout << "Method: " << summary.method << '\n';
    std::cout << "File: " << summary.file << '\n';
    std::cout << "File size: " << summary.file_size << " bytes\n";
    if (summary.block_size == 0)
        std::cout << "Block size: N/A\n";
    else
        std::cout << "Block size: " << summary.block_size << " bytes\n";
    std::cout << "Checksum: " << summary.checksum << '\n';
    std::cout << "Runs: " << summary.times.size() << '\n';
    std::cout << "Min time: " << summary.min_seconds << " s\n";
    std::cout << "Median time: " << summary.median_seconds << " s\n";
    std::cout << "Max time: " << summary.max_seconds << " s\n";
    std::cout << "Median throughput: " << summary.median_throughput_mib_s << " MiB/s\n";
}

void save_benchmark_summary_csv(const std::filesystem::path& csv_path, const std::vector<BenchmarkSummary>& summaries) {
    std::ofstream out(csv_path);
    if (!out)
        throw std::runtime_error("cannot open summary csv");
    out << "file,file_size_bytes,method,block_size_bytes,runs,checksum,min_seconds,median_seconds,max_seconds,median_throughput_mib_s\n";
    out << std::setprecision(12);
    for (const auto& summary : summaries) {
        out << '"' << summary.file << '"' << ',';
        out << summary.file_size << ',';
        out << summary.method << ',';
        out << summary.block_size << ',';
        out << summary.times.size() << ',';
        out << summary.checksum << ',';
        out << summary.min_seconds << ',';
        out << summary.median_seconds << ',';
        out << summary.max_seconds << ',';
        out << summary.median_throughput_mib_s << '\n';
    }
}

void save_benchmark_raw_csv(const std::filesystem::path& csv_path, const std::vector<BenchmarkSummary>& summaries) {
    std::ofstream out(csv_path);
    if (!out)
        throw std::runtime_error("cannot open raw csv");
    out << "file,file_size_bytes,method,block_size_bytes,run,checksum,seconds,throughput_mib_s\n";
    out << std::setprecision(12);
    for (const auto& summary : summaries) {
        for (std::size_t i = 0; i < summary.times.size(); i++) {
            double throughput = calculate_throughput(summary.file_size, summary.times[i]);
            out << '"' << summary.file << '"' << ',';
            out << summary.file_size << ',';
            out << summary.method << ',';
            out << summary.block_size << ',';
            out << i + 1 << ',';
            out << summary.checksum << ',';
            out << summary.times[i] << ',';
            out << throughput << '\n';
        }
    }
}

}