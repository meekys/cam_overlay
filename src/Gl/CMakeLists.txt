cmake_minimum_required(VERSION 2.8.9)
project(Gl)

find_package(OpenGL REQUIRED COMPONENTS EGL)

include_directories(
    ${OPENGL_INCLUDE_DIR}
    ${OPENGL_EGL_INCLUDE_DIRS})

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(gl ${source_files})

target_link_libraries(gl core)
target_include_directories(gl PUBLIC include ${OPENGL_INCLUDE_DIR})

target_link_libraries(
    gl
    ${OPENGL_egl_LIBRARY}
    GLESv2)