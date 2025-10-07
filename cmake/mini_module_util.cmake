macro (generate_api_name name)
    string(REGEX REPLACE "mini[._]" "" api ${name})
    string(REPLACE "." "_" api ${api})
    string(TOUPPER ${api} api_upper)
endmacro()

function (generate_module_entry name)
    generate_api_name(${name})
    snake_to_camel_case(${api})
    
    string(CONCAT class "mini::" ${api} "::ModuleInterface")
    string(CONCAT namespace "mini::" ${api})
    set(file_path "${CMAKE_CURRENT_BINARY_DIR}/${api}.generated.cpp")

    set(args API CLASS FILE_PATH CUSTOM_UNIT_GENERATOR)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "${args}" "")

    if (DEFINED arg_API)
        set(api ${arg_API})
        string(TOUPPER ${api} api_upper)
    endif()
    if (DEFINED arg_CLASS)
        set(class ${arg_CLASS})
    endif()
    if (DEFINED arg_FILE_PATH)
        set(file_path ${arg_FILE_PATH})
    endif()

    configure_file(
        ${ENGINE_PROJECT_DIR}/cmake/template/module_entry.cpp
        ${file_path}
        @ONLY
        NEWLINE_STYLE LF
    )

    get_target_property(target_type ${name} TYPE)
    if (target_type STREQUAL "STATIC_LIBRARY")
        target_compile_definitions(${name} PRIVATE ${api_upper}_STATIC=1)
        set_property(DIRECTORY ${ENGINE_SOURCE_DIR} APPEND PROPERTY static_modules ${name})
    elseif (target_type STREQUAL "SHARED_LIBRARY")
        target_compile_definitions(${name} PRIVATE ${api_upper}_STATIC=0)
    else()
        message(FATAL_ERROR "unsupported build type: " ${target_type})
    endif()

    target_sources(${name} PRIVATE ${file_path})
endfunction()

function (generate_init_file target module_list)
    foreach (module ${module_list})
        generate_api_name(${module})
        snake_to_camel_case(${api})
        list(APPEND helper_list ${camel_case}ModuleInitializeHelper)
        list(APPEND contents "import ${module}\;\n")
    endforeach()
    list(APPEND contents "\n")

    foreach (helper ${helper_list})
        list(APPEND contents "void ${helper}()\;\n")
    endforeach()

    list(APPEND contents "\nvoid InvokeStaticModuleInitializeHelpers()\n{\n")
    foreach (helper ${helper_list})
        list(APPEND contents "    ${helper}()\;")
    endforeach()
    list(APPEND contents "\n}\n")

    set(file_path "${CMAKE_CURRENT_BINARY_DIR}/init.generated.cpp")
    file(WRITE ${file_path} ${contents})
    target_sources(${target} PRIVATE ${file_path})
endfunction()

function (build_source_tree name)
    generate_api_name(${name})
    
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
        string(GENEX_STRIP ${file} generator_removed)
        if (${generator_removed} MATCHES "impl/.*")
            list(APPEND implementations ${generator_removed})
        else()
            list(APPEND files ${generator_removed})
        endif()
    endforeach()

    get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    set(cmake_gen_path ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${name}.dir)
    list(APPEND generated ${cmake_gen_path}/cmake_pch.cxx)
    if (is_multi_config)
        foreach(config_type ${CMAKE_CONFIGURATION_TYPES})
            list(APPEND generated ${cmake_gen_path}/${config_type}/cmake_pch.hxx)
        endforeach()
    else()
        list(APPEND generated ${cmake_gen_path}/cmake_pch.hxx)
    endif()

    list(REMOVE_ITEM files ${CMAKE_CURRENT_BINARY_DIR}/${api}.generated.cpp)
    list(REMOVE_ITEM files ${CMAKE_CURRENT_BINARY_DIR}/${api}.generated.h)
    list(REMOVE_ITEM files ${CMAKE_CURRENT_BINARY_DIR}/init.generated.cpp)
    list(APPEND generated ${CMAKE_CURRENT_BINARY_DIR}/${api}.generated.cpp)
    list(APPEND generated ${CMAKE_CURRENT_BINARY_DIR}/${api}.generated.h)
    list(APPEND generated ${CMAKE_CURRENT_BINARY_DIR}/init.generated.cpp)

    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/impl PREFIX "Implementation Files" FILES ${implementations})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${files})
    source_group("Generated Files" FILES ${generated})
endfunction()