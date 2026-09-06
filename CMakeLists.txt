cmake_minimum_required(VERSION 3.20)
project(large_file_io_lab LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #for clangd
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_MakeAvailable(googletest)

add_library(largeio
    src/file_utils.cpp
)

target_include_directories(largeio
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(largeio PRIVATE /W4)
else()
    target_compile_options(largeio PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

add_executable(largeio_smoke
    src/main.cpp
)

target_link_libraries(largeio_smoke
    PRIVATE
        largeio
)

enable_testing()

add_executable(largeio_tests
    tests/test_file_utils.cpp
)

target_link_libraries(largeio_tests
    PRIVATE
        GTest::gtest_main
        largeio
)

include(GoogleTest)

gtest_discover_tests(largeio_tests)