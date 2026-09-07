#pragma once
//#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <utility>
#include <sys/stat.h>
#include <sys/mman.h>
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
        // запрет копирование тк fd не может быть скопирован только перемещен (a.fd = 3 b.fd = 3 нереален ) 
        FileDescriptor(const FileDescriptor&) = delete;
        FileDescriptor& operator=(const FileDescriptor&) = delete;
        // разрешаем перемещение
        FileDescriptor(FileDescriptor&& other) noexcept : _fd(std::exchange(other._fd, -1)) {}
        FileDescriptor& operator=(FileDescriptor&& other) noexcept {
            if (this != &other) {
                if (_fd != -1) {
                    ::close(_fd);
                }
                _fd = std::exchange(other._fd, -1);
            }
            return *this;
        }
    };

    bool is_regular_file(const fs::path& path);

    std::string get_current_path();

    fs::path get_parent_path (const fs::path& path);

    fs::path get_square_parent_path(const fs::path& path );

    fs::path get_data_path_from_test(); 

    fs::path get_test_data_path(const std::string);
}