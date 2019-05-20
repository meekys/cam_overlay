cmake_minimum_required(VERSION 2.8.9)
project(OutputFile)

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(OutputFile ${source_files})

target_include_directories(OutputFile PUBLIC include)

target_link_libraries(
    OutputFile
    camoverlay)

list(APPEND output_modules "File")