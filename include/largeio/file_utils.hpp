#pragma once
#include <fcntl.h>
#include <unistd.h>
#include <filesystem>
namespace fs =  std::filesystem;

namespace largeio {

    // RAII паттерн )_))))
    class FileDescriptor {
    private:
        int _fd;  
    public:
        explicit FileDescriptor(int fd = -1) noexcept : _fd(fd) {}
        ~FileDescriptor() {
            if (_fd != -1) {
                ::close(_fd);
            }
        }
        int get() const noexcept { return _fd; }
    };

    bool is_regular_file(const fs::path& path);

}