#include <iostream>
#include <filesystem>
#define vec vector
namespace fs =  std::filesystem;

bool is_regular_file(fs::path path) {
	if (!fs::exists(path) || !fs::is_regular_file(path)) {
		std::cerr << "not a regular file \n";
		return 0;
	}
	return 1;
}


signed main(int argc, char **argv) {
	
}