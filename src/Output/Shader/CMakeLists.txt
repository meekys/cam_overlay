cmake_minimum_required(VERSION 2.8.9)
project(OutputShader)

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE resource_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/res/*.*)

add_library(OutputShader ${source_files})

target_include_directories(OutputShader PUBLIC include)

target_link_libraries(
    OutputShader
    camoverlay)

target_copy_files(OutputShader "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" "${resource_files}")

list(APPEND output_modules "Shader")