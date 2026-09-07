#include <filesystem>
#include <largeio/file_utils.hpp>
#include <string>
//#define vec vector

namespace largeio {

bool is_regular_file(const fs::path& path) {
    return fs::exists(path) && fs::is_regular_file(path);
}

std::string get_current_path() {
    return __FILE__;
}

fs::path get_parent_path(const fs::path& path) {
    return path.parent_path();
}

fs::path get_square_parent(const fs::path& path) {
    return path.parent_path().parent_path();
}

fs::path get_data_path_from_test() {
    return get_square_parent(get_current_path()) / "data";
}

fs::path get_test_data_path(const std::string filename) {
    return get_square_parent(get_current_path()) / "tests" / "test_data" / filename;
}

}