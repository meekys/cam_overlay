cmake_minimum_required(VERSION 2.8.9)
project(OutputShader)

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(OutputShader ${source_files})

target_include_directories(OutputShader PUBLIC include)

target_link_libraries(
    OutputShader
    camoverlay)

list(APPEND output_modules "Shader")