#include "largeio/file_utils.hpp"
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
    if (block_size == 0) {
        std::cerr << "Error: block_size must be greater than 0.\n";
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

std::uint64_t checksum_fd(const fs::path& path, std::size_t block_size) {
    if (!largeio::is_regular_file(path)) {
        std::cerr << "Error: " << path << " is not a regular file.\n";
        return 0;
    }
    if (block_size == 0) {
        std::cerr << "Error: block_size must be greater than 0.\n";
        return 0;
    }
    //int fd=::open(path.c_str(), O_RDONLY);
    FileDescriptor fd(::open(path.c_str(), O_RDONLY));
    if (fd.get() == -1) {
        std::cerr << "Error opening file: " << path << '\n';
        return 0;
    }
    std::vector<unsigned char> buffer(block_size);
    std::uint64_t checksum = 0;
    for (;;) {
        ssize_t readd = ::read(fd.get(),buffer.data(),static_cast<ssize_t>(buffer.size()));
        if (readd == 0) break;
        if (readd == -1) {
            std::cerr << "Error reading file: " << path << '\n';
            break;
        }
        for (ssize_t i = 0; i < readd; i ++) 
            checksum += buffer[static_cast<std::size_t>(i)];
    }
    //::close(fd.get());
    return checksum;
}

std::uint64_t checksum_mmap(const fs::path& path) {
    if (!largeio::is_regular_file(path)) {
        std::cerr << "Error: " << path << " is not a regular file.\n";
        return 0;
    }
    /*if (block_size == 0) {
        std::cerr << "Error: block_size must be greater than 0.\n";
        return 0;
    }*/
    FileDescriptor fd(::open(path.c_str(), O_RDONLY));
    if (fd.get() == -1) {
        std::cerr << "Error opening file: " << path << '\n';
        return 0;
    }
    std::uint64_t checksum = 0;
    struct stat file_stat{};
    if (::fstat(fd.get(), &file_stat) == -1) {
        std::cerr << "Error getting file size: " << path << '\n';
        return 0;
    }
    if (file_stat.st_size == 0) {
        return 0; // пустой
    }
    void* mapped = ::mmap(nullptr, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd.get(), 0);
    if (mapped == MAP_FAILED) {
        std::cerr << "Error mapping file: " << path << '\n';
        return 0;
    }
    const unsigned char* data = static_cast<const unsigned char*>(mapped);
    for (std::size_t i = 0; i < file_stat.st_size; i++) {
        checksum += data[i];
    }
    ::munmap(mapped, file_stat.st_size);
    return checksum;
}

}