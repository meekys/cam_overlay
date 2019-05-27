cmake_minimum_required(VERSION 2.8.9)
project(DisplayBcm)

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(DisplayBcm ${source_files})

target_include_directories(DisplayBcm PUBLIC include)

include_directories(${BCM_INCLUDE_DIR})

target_link_libraries(
    DisplayBcm
    camoverlay
    gl
    ${BCM_LIBRARY}
    ${BCM_EGL_LIBRARY})

list(APPEND display_modules "Bcm")
