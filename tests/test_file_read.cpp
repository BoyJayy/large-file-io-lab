#include <gtest/gtest.h>
#include <largeio/file_read.hpp>
#include <largeio/file_utils.hpp>
#include "test_helpers.hpp"
#include <array>
#include <cstdint>
#include <filesystem>
#include <string>

namespace largeio {

namespace fs = std::filesystem;

TEST(FileReadTest, SimplestReadFileReadsKnownText) {
    const auto path = test_helpers::test_data_path("readtest1.txt");
    ASSERT_TRUE(fs::exists(path));
    ASSERT_TRUE(largeio::is_regular_file(path));
    const std::string content = simplest_read_file(path);
    ASSERT_NE(content, "error");
    EXPECT_EQ(content,"hello hello hello\nhello\n" );
}

TEST(FileReadTest, EmptyFileHasZeroChecksum) {
    const auto path = test_helpers::data_path("empty.bin");
    ASSERT_TRUE(fs::exists(path)) << "Run scripts/generate_data.py first";
    EXPECT_EQ(checksum_ifstream(path, 4096), 0);
    EXPECT_EQ(checksum_fd(path, 4096), 0);
    EXPECT_EQ(checksum_mmap(path), 0);
}

TEST(FileReadTest, SmallFileHasKnownChecksum) {
    const auto path =test_helpers::data_path("small.bin");
    ASSERT_TRUE(fs::exists(path)) << "Run scripts/generate_data.py first";
    ASSERT_EQ(fs::file_size(path), 31);
    // 0, 1, 2, ..., 30
    // 0 + 1 + ... + 30 = 465
    constexpr std::uint64_t expected = 465;
    EXPECT_EQ(checksum_ifstream(path, 8), expected);
    EXPECT_EQ(checksum_fd(path, 8),expected );
    EXPECT_EQ( checksum_mmap(path), expected);
}

TEST(FileReadTest, HandlesPartialFinalBlock) {
    const auto path = test_helpers::data_path("small.bin");
    ASSERT_TRUE(fs::exists(path));
    constexpr std::uint64_t expected = 465;
    // 31 bytes:
    // block = 8
    //
    // read:
    // 8
    // 8
    // 8
    // 7
    EXPECT_EQ(checksum_ifstream(path, 8),expected);
    EXPECT_EQ(checksum_fd(path, 8),expected);
}

TEST(FileReadTest, BufferCanBeLargerThanFile) {
    const auto path = test_helpers::data_path("small.bin");
    constexpr std::uint64_t expected = 465;
    EXPECT_EQ(checksum_ifstream(path, 4096),expected );
    EXPECT_EQ(checksum_fd(path, 4096),expected);
}

TEST(FileReadTest, ChecksumDoesNotDependOnBlockSize) {
    const auto path =test_helpers::data_path("small.bin");
    ASSERT_TRUE(fs::exists(path));
    constexpr std::array<std::size_t, 7> block_sizes{1,2,7,8,16,31,4096};
    constexpr std::uint64_t expected = 465;
    for (const auto block_size : block_sizes) {
        SCOPED_TRACE("block_size = " +std::to_string(block_size));
        EXPECT_EQ(checksum_ifstream(path, block_size), expected);
        EXPECT_EQ(  checksum_fd(path, block_size),  expected);
    }
}

TEST(FileReadTest, AllReadersProduceSameChecksum) {
    struct TestCase {const char* filename;std::uintmax_t expected_size;
    };
    constexpr std::array<TestCase, 6> cases{{
       {"one_byte.bin",1},
        {"small.bin",31},
        {"4KiB.bin", 4 * 1024},
        {"4KiB_plus_1.bin",4 * 1024 + 1},
        {"64KiB.bin", 64 * 1024},
        {"1MiB.bin",1024 * 1024}
    }};
    constexpr std::size_t block_size =64 * 1024;
    for (const auto& test_case : cases) {
        SCOPED_TRACE(test_case.filename);
        const auto path =test_helpers::data_path(test_case.filename );
        ASSERT_TRUE(fs::exists(path)) << "Missing " << path << ". Run scripts/generate_data.py";
        ASSERT_EQ(fs::file_size(path),test_case.expected_size);
        const std::uint64_t expected = test_helpers::expected_pattern_checksum(    test_case.expected_size);
        const auto ifstream_result = checksum_ifstream(   path,  block_size);
        const auto fd_result =checksum_fd(path, block_size );
        const auto mmap_result =checksum_mmap(path);
        EXPECT_EQ(ifstream_result, expected);
        EXPECT_EQ(fd_result, expected);
        EXPECT_EQ(mmap_result, expected);
        EXPECT_EQ(ifstream_result, fd_result);
        EXPECT_EQ(fd_result, mmap_result);
    }
}

}