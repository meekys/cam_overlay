cmake_minimum_required(VERSION 2.8.9)
project(InputFile)

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(InputFile ${source_files})

target_include_directories(InputFile PUBLIC include)

target_link_libraries(
    InputFile
    core
    camoverlay)

list(APPEND input_modules "File")

set(input_modules "${input_modules}" PARENT_SCOPE)