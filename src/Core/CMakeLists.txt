cmake_minimum_required(VERSION 2.8.9)
project(Core)

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(core ${source_files})

target_include_directories(core PUBLIC include)