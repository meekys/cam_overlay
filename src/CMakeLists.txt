cmake_minimum_required(VERSION 2.8.9)

include(CopyFiles.cmake)
include(RaspberryPi.cmake)

add_subdirectory(Core)
add_subdirectory(Gl)
add_subdirectory(CamOverlay)
add_subdirectory(Display)
add_subdirectory(Input)
add_subdirectory(Output)
add_subdirectory(App)