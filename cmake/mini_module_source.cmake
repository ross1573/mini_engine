include (mini_util)

function (build_source_tree target)
    get_target_property(source_dir ${target} SOURCE_DIR)
    get_target_property(binary_dir ${target} BINARY_DIR)
    get_target_property(sources ${target} SOURCES)
    get_target_property(headers ${target} HEADERS)

    # search for HEADER_SETS
    # if its named "generated", it will be treated as generated file set
    get_target_property(header_sets ${target} HEADER_SETS)
    foreach (header_set IN LISTS header_sets)
        get_target_property(header_files ${target} HEADER_SET_${header_set})
        string(TOUPPER ${header_set} header_set_upper)

        if (header_set_upper MATCHES "^GENERATED")
            list(APPEND generated_files ${header_files})
        else()
            list(APPEND headers ${header_files})
        endif()
    endforeach()

    # search for CXX_MODULE_SETS
    # if its named "generated", it will be treated as generated file set
    get_target_property(module_sets ${target} CXX_MODULE_SETS)
    foreach (module_set IN LISTS module_sets)
        get_target_property(module_files ${target} CXX_MODULE_SET_${module_set})
        string(TOUPPER ${module_set} module_set_upper)

        if (module_set_upper MATCHES "^GENERATED")
            list(APPEND generated_files ${module_files})
        else()
            list(APPEND modules ${module_files})
        endif()
    endforeach()

    # filter out generator expression 
    # this will combine all files into a single list
    foreach (file IN LISTS sources headers modules)
        # pass empty file set cases
        if (file STREQUAL headers-NOTFOUND)
            continue()
        elseif (file STREQUAL modules-NOTFOUND)
            continue()
        endif()

        string(REGEX MATCHALL "\\$<.*:(.*)>" generator_removed ${file})

        # without generator expression
        if (${CMAKE_MATCH_COUNT} EQUAL 0)        
            list(APPEND filtered_list "${file}")
            continue()
        endif()

        # store all match cases
        foreach (loopVar RANGE 1 ${CMAKE_MATCH_COUNT})
            set(matchVar "CMAKE_MATCH_${loopVar}")
            string(REPLACE "," ";" condition_list ${${matchVar}})
            list(APPEND filtered_list ${condition_list})
        endforeach()
    endforeach()

    get_property(module_list GLOBAL PROPERTY MODULE_LIST)
    foreach (file ${filtered_list})
        if (${file} IN_LIST module_list)
            continue()
        endif()

        if (IS_ABSOLUTE ${file})
            set(path ${file})
        else()
            cmake_path(ABSOLUTE_PATH file 
                BASE_DIRECTORY ${source_dir} 
                NORMALIZE 
                OUTPUT_VARIABLE path)
        endif()

        # filter generated source file
        get_source_file_property(generated ${path} GENERATED)
        if (generated)
            list(APPEND generated_files ${path})
            continue()
        endif()

        # filter implementation files
        if (${path} MATCHES "^impl/.*|.*/impl/.*")
            list(APPEND implementations ${path})
        else()
            list(APPEND files ${path})
        endif()
    endforeach()

    # make a list of generated files
    set(cmake_gen_path "${binary_dir}/CMakeFiles/${target}.dir")
    get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if (is_multi_config)
        list(APPEND generated_files "${cmake_gen_path}/cmake_pch.cxx")
        list(APPEND generated_files "${cmake_gen_path}/$<CONFIG>/cmake_pch.hxx")

        # convert config generator into a real configuration
        foreach (gen_file ${generated_files})
            if (NOT gen_file MATCHES ".*\\$\<CONFIG\>.*")
                continue()
            endif()

            foreach(config_type ${CMAKE_CONFIGURATION_TYPES})
                string(REPLACE "$<CONFIG>" "${config_type}" configured_file ${gen_file})
                list(APPEND generated_files ${configured_file})
            endforeach()
        endforeach()
    else()
        list(APPEND generated_files "${cmake_gen_path}/cmake_pch.cxx")
        list(APPEND generated_files "${cmake_gen_path}/cmake_pch.hxx")
    endif()

    # build source tree
    source_group(TREE ${source_dir} PREFIX "Implementation Files" FILES ${implementations})
    source_group(TREE ${source_dir} PREFIX "Source Files" FILES ${files})
    source_group("Generated Files" FILES ${generated_files})
endfunction()

function (module_sources target)
    target_sources(${ARGV})
    set_target_properties(${target} PROPERTIES BUILD_SOURCE_TREE_FLAG ON)
    set_property(GLOBAL APPEND PROPERTY BUILD_SOURCE_TREE_TARGET ${target})
endfunction()

function (exec_build_source_tree)
    timer_start(build_source_tree)
    get_property(build_list GLOBAL PROPERTY BUILD_SOURCE_TREE_TARGET)
    
    foreach (target ${build_list})
        get_target_property(build_flag ${target} BUILD_SOURCE_TREE_FLAG)
        if (NOT build_flag)
            continue()
        endif()

        build_source_tree(${target})
        set_target_properties(${target} PROPERTIES BUILD_SOURCE_TREE_FLAG OFF)
    endforeach()

    timer_end(build_source_tree)
    timer_print("Building source tree" build_source_tree)
endfunction()