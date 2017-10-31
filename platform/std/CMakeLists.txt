cmake_minimum_required(VERSION 3.8)
project(std)

set(CMAKE_CXX_STANDARD 11)

include_directories(../..)

find_library(GEF_LIBRARY gef
        PATHS ../../cmake-build-debug)

set(SOURCE_FILES system/debug_log_std.cpp system/file_std.cpp system/file_std.h)
add_library(std ${SOURCE_FILES})

target_link_libraries(std $(GEF_LIBRARY))



