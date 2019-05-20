cmake_minimum_required(VERSION 2.8.9)
project(CamOverlayApp)

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)

function(add_modules filename type projects modules)
    set(contents "")

    foreach(project ${projects})
        list(APPEND contents "#include <${project}/${project}.hpp>\n")
    endforeach(project)

    list(APPEND contents "\n")

    string(TOUPPER "${type}" typeUpper)
    string(TOLOWER "${type}" typeLower)
    list(APPEND contents "#define ${typeUpper}_MODULES \\\n")

    foreach(module ${modules})
        list(APPEND contents "  result = Check${type}<${type}${module}>(injector, config, \"${module}\")\; \\\n")
        list(APPEND contents "  \\\n")
        list(APPEND contents "  if (result != nullptr) \\\n")
        list(APPEND contents "    return result\; \\\n")
    endforeach(module)

    list(APPEND contents "// ${typeUpper}_MODULES\n")

    file(WRITE ${filename} ${contents})
endfunction(add_modules)

set(modules_header )

add_modules("${PROJECT_SOURCE_DIR}/src/Generated/DisplayModules.hpp" Display "${display_projects}" "${display_modules}")
add_modules("${PROJECT_SOURCE_DIR}/src/Generated/InputModules.hpp" Input "${input_projects}" "${input_modules}")
add_modules("${PROJECT_SOURCE_DIR}/src/Generated/OutputModules.hpp" Output "${output_projects}" "${output_modules}")

message("display_projects ${display_projects}")
message("input_projects ${input_projects}")
message("output_projects ${output_projects}")

message("display_modules ${display_modules}")
message("input_modules ${input_modules}")
message("output_modules ${output_modules}")

add_executable(camoverlay.bin ${source_files})

#function(prepend var prefix)
#    SET(listVar "")
#    FOREACH(f ${${var}})
#        LIST(APPEND listVar "${prefix}${f}")
#    ENDFOREACH(f)
#    SET(${var} "${listVar}" PARENT_SCOPE)
#endfunction(prepend)

target_link_libraries(
    camoverlay.bin
    camoverlay
    ${display_projects}
    ${input_projects}
    ${output_projects})