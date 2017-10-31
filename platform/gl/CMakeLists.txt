cmake_minimum_required(VERSION 3.8)
project(gl)

set(CMAKE_CXX_STANDARD 11)

include_directories(../..)

find_library(GEF_LIBRARY gef
        PATHS ../../cmake-build-debug)


set(SOURCE_FILES system/platform_gl.cpp system/platform_gl.h graphics/vertex_buffer_gl.cpp graphics/vertex_buffer_gl.h graphics/index_buffer_gl.cpp graphics/index_buffer_gl.h graphics/texture_gl.cpp graphics/texture_gl.h graphics/shader_interface_gl.cpp graphics/shader_interface_gl.h graphics/sprite_renderer_gl.cpp graphics/sprite_renderer_gl.h graphics/renderer_3d_gl.cpp graphics/renderer_3d_gl.h graphics/render_target_gl.cpp graphics/render_target_gl.h)
add_library(gl ${SOURCE_FILES})

target_link_libraries(gl $(GEF_LIBRARY))