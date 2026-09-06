#pragma once
#include <largeio/file_utils.hpp>
#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string>

namespace largeio {

//используем обычный fstream для чтения файлов 
std::string simplest_read_file(const fs::path& filename);

//бинарное блоковое чтение
// разница в ассимптотике по памяти в простейшем случае было бы O(n) но щас O(block_size) но в отличие от простейшего вернется только сумма байтов
std::uint64_t checksum_ifstream(const fs::path& filename, std::size_t block_size );

//posix file descriptor checksum read
std::uint64_t checksum_fd(const fs::path& filename, std::size_t block_size);

}