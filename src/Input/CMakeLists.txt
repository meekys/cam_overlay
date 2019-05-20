cmake_minimum_required(VERSION 2.8.9)

function(add_input_project name)
    add_subdirectory(${name})

    list(APPEND input_projects "Input${name}")
    set(input_projects "${input_projects}" PARENT_SCOPE)
    set(input_modules "${input_modules}" PARENT_SCOPE)
endfunction(add_input_project)

add_input_project(V4L2)
add_input_project(File)

set(input_modules "${input_modules}" PARENT_SCOPE)
set(input_projects "${input_projects}" PARENT_SCOPE)
