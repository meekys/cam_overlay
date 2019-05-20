cmake_minimum_required(VERSION 2.8.9)
project(InputV4L2)

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(InputV4L2 ${source_files})

target_include_directories(InputV4L2 PUBLIC include)

target_link_libraries(
    InputV4L2
    camoverlay)

list(APPEND input_modules "V4L2MMap")
list(APPEND input_modules "V4L2UserPtr")
list(APPEND input_modules "V4L2Read")

set(input_modules "${input_modules}" PARENT_SCOPE)