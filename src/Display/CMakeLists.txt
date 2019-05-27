cmake_minimum_required(VERSION 2.8.9)

function(add_display_project name)
    add_subdirectory(${name})

    list(APPEND display_projects "Display${name}")
    list(APPEND display_modules "${name}")
    set(display_projects "${display_projects}" PARENT_SCOPE)
    set(display_modules "${display_modules}" PARENT_SCOPE)
endfunction(add_display_project)

find_package(X11)

if(${RASPBERRY_PI})
  add_display_project(Bcm)
endif()

if(X11_FOUND)
    add_display_project(X11)
endif()

set(display_modules "${display_modules}" PARENT_SCOPE)
set(display_projects "${display_projects}" PARENT_SCOPE)