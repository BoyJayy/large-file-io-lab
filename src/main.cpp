#include <iostream>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <largeio/file_utils.hpp>
#include <largeio/file_read.hpp>
namespace fs = std::filesystem;

signed main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "usage: app <file>\n";
        return 0;
    }
    fs::path path = argv[1];
    if (!largeio::is_regular_file(path)) {
        std::cerr << "not a regular file \n";
        return 0;
    }
    std::cout << "file: " << path << '\n';
    std::cout << "size: " << fs::file_size(path) << " bytes\n";
    //std::cout << "content: \n" << largeio::simplest_read_file(path) << '\n';
    //std::cout << "checksum: " << largeio::checksum_ifstream(path, 1024*1024) << '\n';
    /*int fd = ::open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        std::cerr << "Error opening file: " << path << '\n';
    } */
    //std::cout << fd << '\n';


    //::close(fd);

    std::cout << "checksum: " << largeio::checksum_fd(path, 1024*1024) << '\n';
    return 0;
}