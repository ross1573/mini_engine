function (build_source_tree name)
    get_target_property(sources ${name} SOURCES)
    get_target_property(headers ${name} HEADERS)

    get_target_property(header_sets ${name} HEADER_SETS)
    foreach (header_set IN LISTS header_sets)
        get_target_property(header_files ${name} HEADER_SET_${header_set})
        list(APPEND headers ${header_files})
    endforeach()

    get_target_property(module_sets ${name} CXX_MODULE_SETS)
    foreach (module_set IN LISTS module_sets)
        get_target_property(module_files ${name} CXX_MODULE_SET_${module_set})
        list(APPEND modules ${module_files})
    endforeach()

    foreach (file IN LISTS sources headers modules)
        string(REGEX REPLACE "\\$<.*:(.*)>" "\\1" generator_removed ${file})
        list(APPEND files ${generator_removed})
    endforeach()

    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${files})

    if (MSVC)
        set(cmake_gen_path ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${name}.dir)

        source_group("Generated Files" FILES ${cmake_gen_path}/cmake_pch.cxx)
        foreach(config_type ${CMAKE_CONFIGURATION_TYPES})
            source_group("Generated Files" FILES ${cmake_gen_path}/${config_type}/cmake_pch.hxx)
        endforeach()
    endif()
endfunction()