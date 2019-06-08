cmake_minimum_required(VERSION 2.8.9)

function(add_output_project name)
    add_subdirectory(${name})

    list(APPEND output_projects "Output${name}")
    list(APPEND output_modules "${name}")
    set(output_projects "${output_projects}" PARENT_SCOPE)
    set(output_modules "${output_modules}" PARENT_SCOPE)
endfunction(add_output_project)

add_output_project(Jpeg)
add_output_project(Shader)
add_output_project(File)

set(output_modules "${output_modules}" PARENT_SCOPE)
set(output_projects "${output_projects}" PARENT_SCOPE)
