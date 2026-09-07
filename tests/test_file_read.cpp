#include <gtest/gtest.h>
#include <largeio/file_read.hpp>
#include <largeio/file_utils.hpp>

namespace largeio {

TEST(FileReadTest, SimplestReadFile) {
    fs::path test_file_path = largeio::get_test_data_path("readtest1.txt");
    std::string content = largeio::simplest_read_file(test_file_path);
    ASSERT_NE(content, "error");
    EXPECT_EQ(content, "hello hello hello\nhello\n");
}

}