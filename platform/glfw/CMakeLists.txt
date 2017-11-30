cmake_minimum_required(VERSION 3.9)
project(gef_glfw)

set(CMAKE_CXX_STANDARD 11)

include_directories(../..)

set(SOURCE_FILES system/platform_glfw.cpp system/platform_glfw.h input/input_manager_glfw.cpp input/input_manager_glfw.h input/touch_input_manager_glfw.cpp input/touch_input_manager_glfw.h input/keyboard_glfw.cpp input/keyboard_glfw.h)
add_library(gef_glfw ${SOURCE_FILES})