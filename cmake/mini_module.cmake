include(mini_util)
include(module/mini_module_api)
include(module/mini_module_define)
include(module/mini_module_source)
include(module/mini_module_bundle)
include(module/mini_module_entry)
include(module/mini_module_log)

function (_set_module_defines name)
    set_property(TARGET ${name} APPEND 
    PROPERTY MODULE_DEFINITIONS
        MODULE_NAME="${name}"
        MODULE_PREFIX="${prefix}"
        MODULE_API="${api}"
        "\n"
        ${api_upper}_STATIC=$<IF:$<STREQUAL:$<TARGET_PROPERTY:${name},TYPE>,STATIC_LIBRARY>,true,false>
    )
endfunction()

function (_parse_module_type out)
    if (BUILD_MONOLITHIC)
        set(${out} "STATIC" PARENT_SCOPE)
        return()
    endif()

    if (${ARGC} GREATER 1)
        list(GET ARGN 0 first_arg)
        string(TOUPPER ${first_arg} ${first_arg})
        
        if (${first_arg} MATCHES "^STATIC$|^SHARED$")
            set(${out} ${first_arg} PARENT_SCOPE)
            return()
        endif()
    endif()

    if(${BUILD_SHARED_LIBS})
        set(${out} "SHARED" PARENT_SCOPE)
    else()
        set(${out} "STATIC" PARENT_SCOPE)
    endif()
endfunction()

function (add_module name)
    set(options 
        "NO_DEFINE_HEADER"
        "NO_API_HEADER"
        "NO_MODULE_LOG"
        "NO_MODULE_ENTRY"
    )
    set(args
        "PREFIX" 
        "API"
        "INTERFACE"
    )
    cmake_parse_arguments(PARSE_ARGV 1 arg "${options}" "${args}" "")
    _parse_module_type(${ARGV})

    add_library(${name} ${type})
    generate_api_name(${name} PREFIX ${arg_PREFIX} API ${arg_API})

    if (NOT DEFINED arg_INTERFACE OR arg_INTERFACE STREQUAL "")
        set(arg_INTERFACE "")
    endif()
    
    target_include_directories(${name} 
    PRIVATE 
        "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    set_property(GLOBAL APPEND PROPERTY MODULE_LIST ${name})
    set_target_properties(${name} PROPERTIES 
        FOLDER module
        OUTPUT_NAME "${prefix}.${api}${MODULE_OUTPUT_SUFFIX}"
        INTERFACE "${arg_INTERFACE}"
    )

    if (NOT arg_NO_API_HEADER)
        generate_api_header(${name} PRIVATE PREFIX ${prefix} API ${api})
    endif()
    if (NOT arg_NO_DEFINE_HEADER)
        _set_module_defines(${name})
        generate_define_header(${name} PREFIX ${prefix} API ${api})
    endif()
    if (NOT arg_NO_MODULE_LOG)
        generate_module_log(${name} PREFIX ${prefix} API ${api})
    endif()
    if (NOT arg_NO_MODULE_ENTRY)
        cmake_language(EVAL CODE "
            cmake_language(DEFER CALL generate_module_entry [[${name}]]
                API [[${api}]]
                PREFIX [[${prefix}]]
                INTERFACE [[${arg_INTERFACE}]]
            )"
        )
    endif()

    if (type STREQUAL "SHARED")
        bundle_module(${name})
    endif()

    cmake_language(EVAL CODE "cmake_language(DEFER CALL build_source_tree [[${name}]])")
endfunction()

function (get_modules out type)
    if (NOT type MATCHES "^STATIC$|^SHARED$")
        message(FATAL_ERROR "unsupported type ${type}")
    endif()

    get_property(module_list GLOBAL PROPERTY MODULE_LIST)
    foreach (module IN LISTS module_list)
        get_target_property(module_type ${module} TYPE)
        if (module_type STREQUAL "SHARED_LIBRARY" AND type STREQUAL "SHARED")
            list(APPEND results ${module})
        elseif (module_type STREQUAL "STATIC_LIBRARY" AND type STREQUAL "STATIC")
            list(APPEND results ${module})
        endif()
    endforeach()

    list(REMOVE_DUPLICATES results)
    set(${out} ${results} PARENT_SCOPE)
endfunction()