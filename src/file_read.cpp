#include <largeio/file_read.hpp>

namespace largeio {

std::string simplest_read_file(const fs::path& path) {
    if (!largeio::is_regular_file(path)) {
        std::cerr << "Error: " << path << " is not a regular file.\n";
        return "error";
    }
    std::ifstream in(path);
    if (!in) {
        std::cerr << "Error opening file: " << path << '\n';
        return "error";
    }
    std::string final_string;
    std::string line;
    while (std::getline(in, line)) {
        final_string += line + '\n';
    }
    return final_string;
}

std::uint64_t checksum_ifstream(const fs::path& path, std::size_t block_size) {
    if (!largeio::is_regular_file(path)) {
        std::cerr << "Error: " << path << " is not a regular file.\n";
        return 0;
    }
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        std::cerr << "Error opening file: " << path << '\n';
        return 0;
    }
    std::vector<char> buffer(block_size);
    std::uint64_t checksum = 0;
    while (in) {
        in.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        std::streamsize bytes_read = in.gcount();
        for (std::streamsize i = 0; i < bytes_read; i ++) {
            checksum += static_cast<unsigned char>(buffer[static_cast<std::size_t>(i)]);
        }
    }
    return checksum;
}

}