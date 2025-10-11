include(GenerateExportHeader)

function (generate_api_name target)
    set(args PREFIX API)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "${args}" "")

    if (DEFINED arg_API AND NOT arg_API STREQUAL "")
        #if api name is provided, use it
        set(api ${arg_API})
        if (NOT DEFINED PREFIX)
            set(arg_PREFIX "")
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
    
    set(file_path "${CMAKE_CURRENT_BINARY_DIR}/${target}.generated.cpp")

    configure_file(
        ${ENGINE_PROJECT_DIR}/cmake/template/module_entry.cpp
        ${file_path}
        @ONLY
        NEWLINE_STYLE LF
    )

    # cache build type specific initializers
    # these initializers will be used on code generated
    get_target_property(target_type ${target} TYPE)
    if (target_type STREQUAL "STATIC_LIBRARY")
        target_compile_definitions(${target} PRIVATE ${api_upper}_STATIC=1)
        set_property(GLOBAL APPEND PROPERTY STATIC_MODULE ${target})
        set_property(GLOBAL APPEND PROPERTY STATIC_INIT "__${api_full}_start_module")
    elseif (target_type STREQUAL "SHARED_LIBRARY")
        target_compile_definitions(${target} PRIVATE ${api_upper}_STATIC=0)
    elseif (target type STREQUAL "MODULE_LIBRARY")
        target_compile_definitions(${target} PRIVATE ${api_upper}_STATIC=0)
    else()
        message(FATAL_ERROR "unsupported build type: " ${target_type})
    endif()

    target_sources(${target} PRIVATE ${file_path})
    set_source_files_properties(${file_path} PROPERTIES GENERATED ON)
endfunction()

function (generate_define_header target)
    string(APPEND parsed_list "#pragma once\n\n")
    list(APPEND COMPILE_DEFINITIONS ${ARGN})
    
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

    set(define_header "define.generated.h")
    get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if (is_multi_config)
        set(define_path "${CMAKE_CURRENT_BINARY_DIR}/${target}.define/$<CONFIG>")
    else()
        set(define_path "${CMAKE_CURRENT_BINARY_DIR}/${target}.define")
    endif()

    file(GENERATE
        OUTPUT "${define_path}/${define_header}"
        CONTENT "${parsed_list}"
        TARGET "${target}"
        FILE_PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
        NEWLINE_STYLE LF
    )

    target_include_directories(${target} PRIVATE ${define_path})
    target_precompile_headers(${target} PRIVATE "${define_path}/${define_header}")
    target_sources(${target} PRIVATE
        FILE_SET generated_define TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES "${define_path}/${define_header}"
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
    list(APPEND contents "module\;\n\n")
    foreach (init ${static_init})
        list(APPEND contents "void ${init}()\;\n")
    endforeach()
    foreach (cleanup ${static_cleanup})
        list(APPEND contents "void ${cleanup}()\;\n")
    endforeach()
    list(APPEND contents "\n")

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

    set(file_path "${CMAKE_CURRENT_BINARY_DIR}/init.generated.cxx")
    file(WRITE ${file_path} ${contents})

    target_sources(${target} PUBLIC
        FILE_SET generated_init TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES ${file_path}
    )
endfunction()

function (generate_api_header target)
    set(args PREFIX API)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "${args}" "")

    generate_api_name(${target} API ${arg_API} PREFIX ${arg_PREFIX})

    set(api_header "${CMAKE_CURRENT_BINARY_DIR}/${target}.generated.h")

    generate_export_header(${target}
        BASE_NAME "${api_upper}"
        EXPORT_FILE_NAME ${api_header}
        EXPORT_MACRO_NAME "${api_upper}_API"
        NO_EXPORT_MACRO_NAME "${api_upper}_HIDDEN"
        DEPRECATED_MACRO_NAME "${api_upper}_DEPRECATED"
    )

    target_precompile_headers(${target} PRIVATE ${api_header})
    target_sources(${target} PRIVATE
        FILE_SET generated_api TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES ${api_header}
    )
endfunction()

function (generate_api_log target)
    set(args PREFIX API)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "${args}" "")

    generate_api_name(${target} API ${arg_API} PREFIX ${arg_PREFIX})
    
    snake_to_camel_case(${api})
    set(file_path "${CMAKE_CURRENT_BINARY_DIR}/${target}.log.generated.cxx")

    configure_file(
        ${ENGINE_PROJECT_DIR}/cmake/template/logger.cxx
        ${file_path}
        @ONLY
        NEWLINE_STYLE LF
    )

    target_sources(${target} PUBLIC
        FILE_SET generated_log TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES ${file_path}
    )
endfunction()

function (build_source_tree target)    
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
        string(REGEX MATCHALL "\\$<.*:(.*)>" generator_removed ${file})

        # without generator expression
        if (${CMAKE_MATCH_COUNT} EQUAL 0)
            # pass HEADER_SETS empty case
            if (file STREQUAL headers-NOTFOUND)
                continue()
            endif()

            list(APPEND filtered_list ${file})
            continue()
        endif()

        # store all match cases
        foreach (loopVar RANGE 1 ${CMAKE_MATCH_COUNT})
            set(matchVar "CMAKE_MATCH_${loopVar}")
            string(REPLACE "," ";" condition_list ${${matchVar}})
            list(APPEND filtered_list ${condition_list})
        endforeach()
    endforeach()

    foreach (file ${filtered_list})
        # filter generated source file
        get_source_file_property(generated ${file} GENERATED)
        if (generated)
            list(APPEND generated_files ${file})
            continue()
        endif()

        # filter implementation files
        if (${file} MATCHES "impl/.*")
            list(APPEND implementations ${file})
        else()
            list(APPEND files ${file})
        endif()
    endforeach()

    # make a list of generated files
    set(cmake_gen_path "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target}.dir")
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
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/impl PREFIX "Implementation Files" FILES ${implementations})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${files})
    source_group("Generated Files" FILES ${generated_files})
endfunction()