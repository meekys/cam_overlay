function(target_copy_files target destination sources)
  foreach(source ${sources})
    get_filename_component(source ${source} ABSOLUTE)
    get_filename_component(filename ${source} NAME)
    get_filename_component(dir ${source} DIRECTORY)

    set (output ".")
    while (NOT ${dir} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
      get_filename_component(base_path ${dir} NAME)
      set (output "${base_path}/${output}")
      get_filename_component(dir ${dir} DIRECTORY)
    endwhile()

    set(output "${destination}/${output}/${filename}")
    get_filename_component(output ${output} ABSOLUTE)

    add_custom_command(
      TARGET ${target}
      POST_BUILD
      COMMENT "Copying ${filename}"
      DEPENDS ${source}
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${source} ${output}
    )
  endforeach()
endfunction()