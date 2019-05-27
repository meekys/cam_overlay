cmake_minimum_required(VERSION 2.8.9)

add_compile_options(-Wall -Wextra -Werror) #-pedantic
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)