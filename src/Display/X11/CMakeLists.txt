cmake_minimum_required(VERSION 2.8.9)
project(DisplayX11)

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(DisplayX11 ${source_files})

target_include_directories(DisplayX11 PUBLIC include)

include_directories(${X11_INCLUDE_DIR})

target_link_libraries(
    DisplayX11
    camoverlay
    gl
    ${X11_LIBRARIES})

list(APPEND display_modules "X11")