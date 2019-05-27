cmake_minimum_required(VERSION 2.8.9)
project(CamOverlayLibrary)

find_package(PNG REQUIRED)

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE resource_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/res/*.*)

add_library(camoverlay ${source_files})

target_include_directories(camoverlay PUBLIC include ../../lib)
target_include_directories(camoverlay PRIVATE ${PNG_INCLUDE_DIRS})

target_link_libraries(
    camoverlay
    stdc++fs
    core
    gl
    ${PNG_LIBRARIES})

target_copy_files(camoverlay "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" "${resource_files}")