cmake_minimum_required(VERSION 3.20)
project(large_file_io_lab CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include(FetchContent)
find_package(OpenSSL 3.0 REQUIRED COMPONENTS Crypto)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_library(largeio
    src/file_utils.cpp
)
target_include_directories(largeio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(largeio_smoke largeio)
add_executable(largeio_smoke src/main.cpp)
enable_testing()

add_executable(largeio_tests
    tests/test_file_utils.cpp
)

target_link_libraries(largeio_tests
    GTest::gtest_main
    largeio
)

include(GoogleTest)
gtest_discover_tests(largeio_tests)