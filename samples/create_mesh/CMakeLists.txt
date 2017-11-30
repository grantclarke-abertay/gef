cmake_minimum_required(VERSION 3.8)
project(create_mesh)

find_package(Threads REQUIRED)
find_package(PNG REQUIRED)
find_package(X11 REQUIRED)

find_package(OpenGL REQUIRED)

find_package(GLEW REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)


set(CMAKE_CXX_STANDARD 11)

include_directories(../..)

find_library(GEF_LIBRARY gef
        PATHS ../../cmake-build-debug)
find_library(GEF_PLATFORM_GL_LIBRARY gl
        PATHS ../../platform/gl/cmake-build-debug)
find_library(GEF_PLATFORM_XGL_LIBRARY xgl
        PATHS ../../platform/xgl/cmake-build-debug)
find_library(GEF_PLATFORM_GLFW_LIBRARY gef_glfw
        PATHS ../../platform/glfw/cmake-build-debug)
find_library(GEF_PLATFORM_STD_LIBRARY std
        PATHS ../../platform/std/cmake-build-debug)


set(SOURCE_FILES main_glfw.cpp mesh_app.cpp mesh_app.h)
add_executable(create_mesh ${SOURCE_FILES})

target_link_libraries(create_mesh ${GEF_PLATFORM_GLFW_LIBRARY} ${GEF_LIBRARY} ${GEF_PLATFORM_GL_LIBRARY} ${GEF_PLATFORM_STD_LIBRARY} ${GEF_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${PNG_LIBRARIES} ${X11_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES})