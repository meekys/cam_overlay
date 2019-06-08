cmake_minimum_required(VERSION 2.8.9)
project(OutputJpeg)

find_package(JPEG REQUIRED)

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE resource_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/res/*.*)

add_library(OutputJpeg ${source_files})

target_include_directories(OutputJpeg PUBLIC include)
target_include_directories(camoverlay PRIVATE ${JPEG_INCLUDE_DIR})

target_link_libraries(
    OutputJpeg
    camoverlay
    ${JPEG_LIBRARIES})

target_copy_files(OutputJpeg "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" "${resource_files}")

list(APPEND output_modules "Jpeg")