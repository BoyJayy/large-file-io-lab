#include <largeio/benchmark.hpp>
#include <largeio/file_read.hpp>
#include <chrono>

namespace largeio {

void print_benchmark_result(const BenchmarkResult& result) {
    std::cout << "Method: " << result.method << '\n';
    std::cout << "File size: " << result.file_size << " bytes\n";
    std::cout << "Block size: " << result.block_size << " bytes\n";
    std::cout << "Checksum: " << result.checksum << '\n';
    std::cout << "Time taken: " << result.seconds << " seconds\n";
    std::cout << "Throughput: " << result.throughput_mib_s << " MiB/s\n";
}

BenchmarkResult benchmark_ifstream(const std::filesystem::path& path,std::size_t block_size) {
    BenchmarkResult result;
    result.method = "ifstream";
    result.file_size = std::filesystem::file_size(path);
    result.block_size = block_size;
    auto start = std::chrono::high_resolution_clock::now();
    result.checksum = checksum_ifstream(path, block_size);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    result.seconds = elapsed_seconds.count();
    result.throughput_mib_s = static_cast<double>(result.file_size) / (1024.0 * 1024.0) / result.seconds;
    return result;
}

}