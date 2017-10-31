cmake_minimum_required(VERSION 3.8)
project(xgl)

set(CMAKE_CXX_STANDARD 11)

include_directories(../..)

find_library(GEF_LIBRARY gef
        PATHS ../../cmake-build-debug)
find_library(GEF_PLATFORM_GL_LIBRARY gl
        PATHS ../../platform/gl/cmake-build-debug)


set(SOURCE_FILES system/platform_xgl.cpp system/platform_xgl.h)
add_library(xgl ${SOURCE_FILES})

target_link_libraries(xgl $(GEF_PLATFORM_GL_LIBRARY) $(GEF_LIBRARY))