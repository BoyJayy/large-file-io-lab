#include <largeio/file_utils.hpp>
//#define vec vector
namespace fs =  std::filesystem;

namespace largeio {

bool is_regular_file(const fs::path path) {
    return fs::exists(path) && fs::is_regular_file(path);
}

}