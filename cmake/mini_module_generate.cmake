include(GenerateExportHeader)

function (generate_api_name target)
    set(args PREFIX API)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "${args}" "")

    if (DEFINED arg_API AND NOT arg_API STREQUAL "")
        #if api name is provided, use it
        set(api ${arg_API})
        if (NOT DEFINED arg_PREFIX)
            set(prefix "")
        else()
            set(prefix ${arg_PREFIX})
        endif()
    elseif (DEFINED arg_PREFIX AND NOT arg_PREFIX STREQUAL "")
        set(prefix ${arg_PREFIX})
        string(REGEX REPLACE "^${prefix}[._]" "" api ${target})
    else()
        # if prefix is not provided, use first comma seperated string
        string(REGEX MATCH "^(.*)[.]" match ${target})
        string(REPLACE "${match}" "" api ${target})
        if (NOT DEFINED api OR api STREQUAL "")
            # when target does not contain any commas, set prefix empty
            set(api ${target})
            set(prefix "")
        else()
            set(prefix ${CMAKE_MATCH_1})
        endif()
    endif()

    if (NOT DEFINED api OR api STREQUAL "")
        message(FATAL_ERROR "failed to create api name for ${target}")
    endif()

    string(REPLACE "." "_" api ${api})
    string(TOUPPER ${api} api_upper)
    string(CONCAT api_full ${prefix} "_" ${api})

    set(prefix ${prefix} PARENT_SCOPE)
    set(api ${api} PARENT_SCOPE)
    set(api_upper ${api_upper} PARENT_SCOPE)
    set(api_full ${api_full} PARENT_SCOPE)
endfunction()

function (generate_module_entry target)
    set(options NULL_INTERFACE)
    set(args PREFIX API INTERFACE_CLASS)
    cmake_parse_arguments(PARSE_ARGV 1 arg "${options}" "${args}" "")

    generate_api_name(${target} API ${arg_API} PREFIX ${arg_PREFIX})

    if (arg_NULL_INTERFACE)
        set(null_interface "true")
        set(class "mini::ModuleInterface")
    elseif (NOT DEFINED arg_INTERFACE_CLASS OR arg_INTERFACE_CLASS STREQUAL "")
        messag(FATAL_ERROR "no given interface class name")
    else()
        set(null_interface "false")
        string(CONCAT class ${prefix} "::" ${api} "::" ${arg_INTERFACE_CLASS})
    endif()
    
    set(file_path "${CMAKE_CURRENT_BINARY_DIR}/${target}.generated")
    set(file_name "${target}.entry.generated.cpp")

    configure_file(
        "${ENGINE_PROJECT_DIR}/cmake/template/module_entry.cpp"
        "${file_path}/${file_name}"
        @ONLY
        NEWLINE_STYLE LF
    )

    # cache build type specific initializers
    # these initializers will be used on code generated
    get_target_property(target_type ${target} TYPE)
    if (target_type STREQUAL "STATIC_LIBRARY")
        set_property(GLOBAL APPEND PROPERTY STATIC_MODULE ${target})
        set_property(GLOBAL APPEND PROPERTY STATIC_INIT "__${api_full}_start_module")
    elseif (NOT target_type STREQUAL "SHARED_LIBRARY")
        message(FATAL_ERROR "unsupported build type: " ${target_type})
    endif()

    target_sources(${target} PRIVATE "${file_path}/${file_name}")
    set_source_files_properties("${file_path}/${file_name}" PROPERTIES GENERATED ON)
endfunction()

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

function (generate_static_init target)
    set(args INITIALIZE CLEANUP)
    cmake_parse_arguments(PARSE_ARGV 1 arg "${options}" "${args}" "")

    if (NOT DEFINED args_INITIALIZE OR arg_INITIALIZE STREQUAL "")
        set(args_INITIALIZE "StaticInitialize")
    endif()
    if (NOT DEFINED args_CLEANUP OR args_CLEANUP STREQUAL "")
        set(args_CLEANUP "StaticCleanup")
    endif()

    get_property(static_init GLOBAL PROPERTY STATIC_INIT)
    get_property(static_cleanup GLOBAL PROPERTY STATIC_CLEANUP)

    # define static init list
    if (NOT DEFINED static_init)
        set(static_init "")
    endif()
    if (NOT DEFINED static_cleanup)
        set(static_cleanup "")
    endif()

    # global module section where declarations are written
    if (NOT static_init STREQUAL "" OR NOT static_cleanup STREQUAL "")
        list(APPEND contents "module\;\n\n")
        foreach (init ${static_init})
            list(APPEND contents "void ${init}()\;\n")
        endforeach()
        foreach (cleanup ${static_cleanup})
            list(APPEND contents "void ${cleanup}()\;\n")
        endforeach()
        list(APPEND contents "\n")
    endif()

    # start module and namespace
    string(REPLACE "." "::" namespace ${target})
    list(APPEND contents "export module ${target}:static_init\;\n\n")
    list(APPEND contents "namespace ${namespace} {\n\n")
    
    # write static initializer function
    list(APPEND contents "void ${args_INITIALIZE}()\n{\n")
    foreach (init ${static_init})
        list(APPEND contents "    ${init}()\;\n")
    endforeach()
    list(APPEND contents "}\n\n")

    # write static cleanup function
    list(APPEND contents "void ${args_CLEANUP}()\n{\n")
    foreach (cleanup ${static_cleanup})
        list(APPEND contents "    ${cleanup}()\;\n")
    endforeach()
    list(APPEND contents "}\n\n} // namespace ${namespace}\n")

    set(file_path "${CMAKE_CURRENT_BINARY_DIR}/${target}.generated")
    set(file_name "${target}.init.generated.cxx")
    file(WRITE "${file_path}/${file_name}" ${contents})

    target_sources(${target} PUBLIC
        FILE_SET generated_init TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES "${file_path}/${file_name}"
    )
endfunction()

function (generate_api_header target scope)
    set(args PREFIX API)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "${args}" "")

    generate_api_name(${target} API ${arg_API} PREFIX ${arg_PREFIX})

    set(file_path "${CMAKE_CURRENT_BINARY_DIR}/${target}.generated")
    if (NOT DEFINED prefix OR prefix STREQUAL "")
        set(file_name "${api}.api.generated.h")
    else()
        set(file_name "${prefix}.${api}.api.generated.h")
    endif()

    if (NOT scope STREQUAL PRIVATE AND NOT scope STREQUAL PUBLIC)
        message(FATAL_ERROR "scope of API header can only be either PRIVATE or PUBLIC")
    endif()

    generate_export_header(${target}
        BASE_NAME "${api_upper}"
        EXPORT_FILE_NAME "${file_path}/${file_name}"
        EXPORT_MACRO_NAME "${api_upper}_API"
        NO_EXPORT_MACRO_NAME "${api_upper}_HIDDEN"
        DEPRECATED_MACRO_NAME "${api_upper}_DEPRECATED"
    )

    target_precompile_headers(${target} ${scope} "${file_path}/${file_name}")
    target_sources(${target} ${scope}
        FILE_SET generated_api_${scope} TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES "${file_path}/${file_name}"
    )
endfunction()

function (generate_api_log target)
    set(args PREFIX API)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "${args}" "")

    generate_api_name(${target} API ${arg_API} PREFIX ${arg_PREFIX})
    
    snake_to_camel_case(${api})
    set(file_path "${CMAKE_CURRENT_BINARY_DIR}/${target}.generated")
    set(file_name "${target}.log.generated.cxx")

    configure_file(
        "${ENGINE_PROJECT_DIR}/cmake/template/logger.cxx"
        "${file_path}/${file_name}"
        @ONLY
        NEWLINE_STYLE LF
    )

    target_sources(${target} PUBLIC
        FILE_SET generated_log TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES "${file_path}/${file_name}"
    )
endfunction()