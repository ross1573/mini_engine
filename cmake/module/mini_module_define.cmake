include(module/mini_module_api)

function (generate_define_header target)
    set(args PREFIX API)
    set(multiVal "DEFINITIONS")
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "${args}" "${multiVal}")

    generate_api_name(${target} API ${arg_API} PREFIX ${arg_PREFIX})

    string(APPEND parsed_list "#ifndef ${api_upper}_DEFINE_H\n")
    string(APPEND parsed_list "#define ${api_upper}_DEFINE_H\n\n")
    list(APPEND COMPILE_DEFINITIONS ${arg_DEFINITIONS})

    foreach (definition ${COMPILE_DEFINITIONS})
        # special case on new line
        if (definition STREQUAL "\n")
            string(APPEND parsed_list "\n")
            continue()
        endif()

        # parse string by assign operator
        string(FIND ${definition} "=" index)
        if (index GREATER 0)
            math(EXPR val_start "${index} + 1")
            math(EXPR padding_len "41 - ${index}")

            # add some padding
            if (padding_len LESS 0)
                set(padding_len 0)
            endif()
            string(REPEAT " " ${padding_len} padding)
        else()
            set(padding "")
        endif()

        string(REPLACE "=" "${padding}" definition ${definition})
        string(APPEND parsed_list "#define " ${definition} "\n")
    endforeach()

    string(APPEND parsed_list "\n#endif // ${api_upper}_DEFINE_H")

    set(file_name "${target}.define.generated.h")
    get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if (is_multi_config)
        set(file_path "${CMAKE_CURRENT_BINARY_DIR}/${target}.generated/$<CONFIG>")
    else()
        set(file_path "${CMAKE_CURRENT_BINARY_DIR}/${target}.generated")
    endif()

    file(GENERATE
        OUTPUT "${file_path}/${file_name}"
        CONTENT "${parsed_list}"
        TARGET "${target}"
        FILE_PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
        NEWLINE_STYLE LF
    )

    target_precompile_headers(${target} PRIVATE "${file_path}/${file_name}")
    target_sources(${target} PRIVATE
        FILE_SET generated_define TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES "${file_path}/${file_name}"
    )
endfunction()