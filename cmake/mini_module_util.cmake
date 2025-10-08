include(GenerateExportHeader)

macro (generate_api_name target)
    set(args PREFIX)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" ${args} "")

    if (DEFINED arg_PREFIX)
        set(prefix ${arg_PREFIX})
        string(REGEX REPLACE "^${prefix}[._]" "" api ${target})
    else()
        string(REGEX MATCH "^(.*)[.]" match ${target})
        string(REPLACE "${match}" "" api ${target})
        if (NOT DEFINED api OR api STREQUAL "")
            set(api ${target}) # don't modify
            set(prefix "")
        else()
            set(prefix ${CMAKE_MATCH_1})
        endif()
    endif()

    string(REPLACE "." "_" api ${api})
    string(TOUPPER ${api} api_upper)
    string(CONCAT api_full ${prefix} "_" ${api})
endmacro()

function (generate_module_entry target)
    set(args PREFIX API CLASS)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "${args}" "")

    if (DEFINED arg_PREFIX)
        set(prefix ${arg_PREFIX})
        generate_api_name(${target} PREFIX ${arg_PREFIX})
    else()
        set(prefix "")
        generate_api_name(${target})
    endif()
        
    # override api if specified
    if (DEFINED arg_API)
        set(api ${arg_API})
        string(TOUPPER ${api} api_upper)
    endif()

    # override class if specified
    if (DEFINED arg_CLASS)
        set(class ${arg_CLASS})
    else()
        string(CONCAT class ${prefix} "::" ${api} "::ModuleInterface")
    endif()
    
    set(file_path "${CMAKE_CURRENT_BINARY_DIR}/${target}.generated.cpp")

    configure_file(
        ${ENGINE_PROJECT_DIR}/cmake/template/module_entry.cpp
        ${file_path}
        @ONLY
        NEWLINE_STYLE LF
    )

    get_target_property(target_type ${target} TYPE)
    if (target_type STREQUAL "STATIC_LIBRARY")
        target_compile_definitions(${target} PRIVATE ${api_upper}_STATIC=1)
        set_property(GLOBAL APPEND PROPERTY STATIC_MODULE ${target})
        set_property(GLOBAL APPEND PROPERTY STATIC_INIT "__${api_full}_init_module")
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

function (generate_static_init target)
    get_property(static_init GLOBAL PROPERTY STATIC_INIT)
    get_property(static_cleanup GLOBAL PROPERTY STATIC_CLEANUP)

    if (NOT DEFINED static_init)
        set(static_init "")
    endif()
    if (NOT DEFINED static_cleanup)
        set(static_cleanup "")
    endif()

    list(APPEND contents "module\;\n\n")
    foreach (init ${static_init})
        list(APPEND contents "void ${init}()\;\n")
    endforeach()
    foreach (cleanup ${static_cleanup})
        list(APPEND contents "void ${cleanup}()\;\n")
    endforeach()
    list(APPEND contents "\n")

    string(REPLACE "." "::" namespace ${target})
    list(APPEND contents "export module ${target}:static_init\;\n\n")
    list(APPEND contents "namespace ${namespace} {\n\n")
    
    list(APPEND contents "void StaticInitialize()\n{\n")
    foreach (init ${static_init})
        list(APPEND contents "    ${init}()\;\n")
    endforeach()
    list(APPEND contents "}\n\n")

    list(APPEND contents "void StaticCleanup()\n{\n")
    foreach (cleanup ${static_cleanup})
        list(APPEND contents "    ${cleanup}()\;\n")
    endforeach()
    list(APPEND contents "}\n\n} // namespace ${namespace}\n")

    set(file_path "${CMAKE_CURRENT_BINARY_DIR}/init.generated.cxx")
    file(WRITE ${file_path} ${contents})

    target_sources(${target} PUBLIC
        FILE_SET generated TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES ${file_path}
    )
endfunction()

function (gnerate_api_header target)
    set(args PREFIX FILE_PATH)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "${args}" "")

    if (DEFINED arg_PREFIX)
        set(prefix ${arg_PREFIX})
        generate_api_name(${target} PREFIX ${arg_PREFIX})
    else()
        set(prefix "")
        generate_api_name(${target})
    endif()

    if (DEFINED arg_FILE_PATH)
        set(api_header ${arg_FILE_PATH})
    else()
        set(api_header "${CMAKE_CURRENT_BINARY_DIR}/${target}.generated.h")
    endif()

    generate_export_header(${target}
        BASE_NAME "${api_upper}"
        EXPORT_FILE_NAME ${api_header}
        EXPORT_MACRO_NAME "${api_upper}_API"
        NO_EXPORT_MACRO_NAME "${api_upper}_HIDDEN"
        DEPRECATED_MACRO_NAME "${api_upper}_DEPRECATED"
    )

    target_precompile_headers(${target} PRIVATE ${api_header})
    target_sources(${target} PRIVATE
        FILE_SET generated TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES ${api_header}
    )
endfunction()

function (module_sources target)
    target_sources(${ARGV})
    build_source_tree(${target})
endfunction()

function (build_source_tree target)
    generate_api_name(${target})
    
    get_target_property(sources ${target} SOURCES)
    get_target_property(headers ${target} HEADERS)

    get_target_property(header_sets ${target} HEADER_SETS)
    foreach (header_set IN LISTS header_sets)
        get_target_property(header_files ${target} HEADER_SET_${header_set})
        if (${header_set} STREQUAL generated)
            list(APPEND generated_files ${header_files})
        else()
            list(APPEND headers ${header_files})
        endif()
    endforeach()

    get_target_property(module_sets ${target} CXX_MODULE_SETS)
    foreach (module_set IN LISTS module_sets)
        get_target_property(module_files ${target} CXX_MODULE_SET_${module_set})
        if (${module_set} STREQUAL generated)
            list(APPEND generated_files ${module_files})
        else()
            list(APPEND modules ${module_files})
        endif()
    endforeach()

    foreach (file IN LISTS sources headers modules)
        string(REGEX MATCHALL "\\$<.*:(.*)>" generator_removed ${file})
        if (${CMAKE_MATCH_COUNT} EQUAL 0)
            if (file STREQUAL headers-NOTFOUND)
                continue()
            endif()

            list(APPEND filtered_list ${file})
            continue()
        endif()

        foreach (loopVar RANGE 1 ${CMAKE_MATCH_COUNT})
            set(matchVar "CMAKE_MATCH_${loopVar}")
            list(APPEND filtered_list ${${matchVar}})
        endforeach()
    endforeach()

    foreach (file ${filtered_list})
        get_source_file_property(generated ${file} GENERATED)
        if (generated)
            list(APPEND generated_files ${file})
            continue()
        endif()

        if (${file} MATCHES "impl/.*")
            list(APPEND implementations ${file})
        else()
            list(APPEND files ${file})
        endif()
    endforeach()

    get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if (is_multi_config)
        set(cmake_gen_path "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target}.dir")
        list(APPEND generated_files "${cmake_gen_path}/cmake_pch.cxx")

        foreach(config_type ${CMAKE_CONFIGURATION_TYPES})
            list(APPEND generated_files "${cmake_gen_path}/${config_type}/cmake_pch.hxx")
        endforeach()
    else()
        list(APPEND generated_files "${cmake_gen_path}/cmake_pch.hxx")
    endif()

    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/impl PREFIX "Implementation Files" FILES ${implementations})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${files})
    source_group("Generated Files" FILES ${generated_files})
endfunction()