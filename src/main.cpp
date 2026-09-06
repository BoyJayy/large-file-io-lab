#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

signed main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "usage: app <file>\n";
        return 0;
    }
    fs::path path = argv[1];
    if (!fs::exists(path) || !fs::is_regular_file(path)) {
        std::cerr << "not a regular file \n";
        return 0;
    }

    std::cout << "file: " << path << '\n';
    std::cout << "size: " << fs::file_size(path) << " bytes\n";
    return 0;
}