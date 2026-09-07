#include <gtest/gtest.h>
#include <largeio/file_utils.hpp>
#include "test_helpers.hpp"
#include <fcntl.h>
#include <type_traits>
#include <utility>

namespace largeio {

TEST(FileUtilsTest, ExistingFileIsRegularFile) {
    const auto path = test_helpers::test_data_path("readtest1.txt");
    ASSERT_TRUE(std::filesystem::exists(path));
    EXPECT_TRUE(largeio::is_regular_file(path));
}

TEST(FileUtilsTest, DirectoryIsNotRegularFile) {
    const auto path = test_helpers::project_root() / "tests";
    ASSERT_TRUE(std::filesystem::exists(path));
    EXPECT_FALSE(largeio::is_regular_file(path));
}

TEST(FileUtilsTest, MissingFileIsNotRegularFile) {
    const auto path = test_helpers::test_data_path("this_file_does_not_exist.bin");
    EXPECT_FALSE(largeio::is_regular_file(path));
}

static_assert(
    !std::is_copy_constructible_v<FileDescriptor>
);
static_assert(
    !std::is_copy_assignable_v<FileDescriptor>
);
static_assert(
    std::is_move_constructible_v<FileDescriptor>
);
static_assert(
    std::is_move_assignable_v<FileDescriptor>
);

TEST(FileDescriptorTest, MoveConstructorTransfersOwnership) {
    const auto path =test_helpers::test_data_path("readtest1.txt");
    const int raw_fd = ::open(path.c_str(), O_RDONLY);
    ASSERT_NE(raw_fd, -1);
    FileDescriptor first(raw_fd);
    EXPECT_EQ(first.get(), raw_fd);
    FileDescriptor second(std::move(first));
    EXPECT_EQ(first.get(), -1);
    EXPECT_EQ(second.get(), raw_fd);
    // close() руками НЕ вызываем.
    // second закроет fd в destructor.
}

}