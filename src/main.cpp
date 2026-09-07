#include <largeio/benchmark.hpp>
#include <largeio/file_utils.hpp>

#include <filesystem>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

signed main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: largeio_smoke <file1> [file2 ...]\n";
        return 1;
    }
    try {
        std::vector<largeio::BenchmarkSummary> all_summaries;
        for (int i = 1; i < argc; i++) {
            fs::path path = argv[i];
            if (!largeio::is_regular_file(path)) {
                std::cerr << "Error: " << path << " is not a regular file.\n";
                continue;
            }
            std::cout << "\n=== " << path << " ===\n";
            std::cout << "size: " << fs::file_size(path) << " bytes\n";
            auto summaries = largeio::run_benchmark_suite(path, 7);
            std::cout << "\n SUMMARY \n";
            for (const auto& summary : summaries) {
                largeio::print_benchmark_summary(summary);
                std::cout << '\n';
            }
            all_summaries.insert(all_summaries.end(), summaries.begin(), summaries.end());
        }
        if (!all_summaries.empty()) {
            fs::create_directories("results");
            largeio::save_benchmark_summary_csv("results/benchmark_summary.csv", all_summaries);
            largeio::save_benchmark_raw_csv("results/benchmark_raw.csv", all_summaries);
            std::cout << "Saved results/benchmark_summary.csv\n";
            std::cout << "Saved results/benchmark_raw.csv\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Benchmark error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}