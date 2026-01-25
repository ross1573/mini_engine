include(mini_util)
include(module/mini_module_api)
include(module/mini_module_define)
include(module/mini_module_source)
include(module/mini_module_entry)
include(module/mini_module_log)

macro (set_module_defines name)
    set(module_defines
        MODULE_NAME="${name}"
        MODULE_PREFIX="${prefix}"
        MODULE_API="${api}"
        MODULE_OUTPUT="$<TARGET_FILE_NAME:${name}>"
        MODULE_OUTPUT_PREFIX="$<TARGET_FILE_PREFIX:${name}>${prefix}${BUILD_PREFIX}"
        MODULE_OUTPUT_SUFFIX="$<TARGET_FILE_SUFFIX:${name}>"
        "\n"
        ${api_upper}_STATIC=$<IF:$<STREQUAL:$<TARGET_PROPERTY:${name},TYPE>,STATIC_LIBRARY>,true,false>
    )

    set_target_properties(${name} 
    PROPERTIES 
        MODULE_DEFINITIONS "${module_defines}"
    )
endmacro()

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
    
    if (${ARGC} GREATER 1)
        list(GET ARGN 0 first_arg)
        string(TOUPPER ${first_arg} ${first_arg})
        
        if (${first_arg} MATCHES "^STATIC$|^SHARED$")
            set(type ${first_arg})
        endif()
    endif()

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
        OUTPUT_NAME "${prefix}${BUILD_PREFIX}.${api}"
        INTERFACE "${arg_INTERFACE}"
    )

    if (NOT arg_NO_API_HEADER)
        generate_api_header(${name} PRIVATE 
            PREFIX ${prefix}
            API ${api}
        )
    endif()

    if (NOT arg_NO_API_LOG)
        generate_module_log(${name} 
            PREFIX ${prefix}
            API ${api}
        )
    endif()

    if (NOT arg_NO_DEFINE_HEADER)
        set_module_defines(${name})
        generate_define_header(${name}
            PREFIX ${prefix}
            API ${api}
        )
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

    cmake_language(EVAL CODE
        "cmake_language(DEFER CALL build_source_tree [[${name}]])"
    )
endfunction()