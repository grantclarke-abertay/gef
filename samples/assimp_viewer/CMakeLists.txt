cmake_minimum_required(VERSION 3.8)
project(assimp_viewer)

find_package(Threads REQUIRED)
find_package(PNG REQUIRED)
find_package(X11 REQUIRED)

find_package(OpenGL REQUIRED)

find_package(GLEW REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

find_package(assimp REQUIRED)
find_package(DevIL REQUIRED)

set(CMAKE_CXX_STANDARD 11)

include_directories(. ../..)

find_library(GEF_LIBRARY gef
        PATHS ../../cmake-build-debug)
find_library(GEF_PLATFORM_GL_LIBRARY gl
        PATHS ../../platform/gl/cmake-build-debug)
find_library(GEF_PLATFORM_GLFW_LIBRARY gef_glfw
        PATHS ../../platform/glfw/cmake-build-debug)
find_library(GEF_PLATFORM_STD_LIBRARY std
        PATHS ../../platform/std/cmake-build-debug)


set(SOURCE_FILES
        main_glfw.cpp
        assimp_viewer.cpp
        assimp_viewer.h
        assimp_scene_loader.cpp
        assimp_scene_loader.h
        orbit_camera.cpp
        orbit_camera.h
        imgui_impl_glfw_gl3.cpp
        imgui_impl_glfw_gl3.h
        imgui.cpp
        imgui.h
        imgui_draw.cpp
        addons/imguifilesystem/imguifilesystem.cpp
        addons/imguifilesystem/imguifilesystem.h
        )
add_executable(assimp_viewer ${SOURCE_FILES})

target_link_libraries(
        assimp_viewer
        ${GEF_PLATFORM_GLFW_LIBRARY}
        ${GEF_LIBRARY}
        ${GEF_PLATFORM_GL_LIBRARY}
        ${GEF_PLATFORM_STD_LIBRARY}
        ${GEF_LIBRARY}
        ${CMAKE_THREAD_LIBS_INIT}
        ${PNG_LIBRARIES}
        ${X11_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${ASSIMP_LIBRARIES}
        ${IL_LIBRARIES}
)
