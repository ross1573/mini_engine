include(mini_util)
include(mini_module_util)

function (module_sources name)
    target_sources(${ARGV})
    build_source_tree(${name})
endfunction()

function (add_module name)
    # parse module type
    if (${ARGC} GREATER 1)
        list(GET ARGN 0 first_arg)
        string(TOUPPER ${first_arg} ${first_arg})
        
        if (${first_arg} MATCHES "^STATIC$|^SHARED$|^MODULE$|^INTERFACE$")
            set(type ${first_arg})
        endif()
    endif()

    set(options 
        "NO_DEFINE_HEADER"
        "NO_API_HEADER"
        "NO_API_LOG" 
        "NO_MODULE_ENTRY"
        "NO_MODULE_INTERFACE"
    )
    set(args
        "PREFIX" 
        "API" 
        "INTERFACE_CLASS"
    )
    cmake_parse_arguments(PARSE_ARGV 1 arg "${options}" "${args}" "")

    add_library(${name} ${type})

    generate_api_name(${name} API ${arg_API} PREFIX ${arg_PREFIX})

    if (NOT arg_NO_DEFINE_HEADER)
        generate_define_header(${name}
            MODULE_NAME="${name}"
            MODULE_PREFIX="${prefix}${BUILD_PREFIX}"
            MODULE_API="${api}"
            MODULE_OUTPUT="$<TARGET_FILE_NAME:${name}>"
        )
    endif()

    if (NOT arg_NO_API_HEADER)
        generate_api_header(${name} API ${api} PREFIX ${prefix})
    endif()

    if (NOT arg_NO_API_LOG)
        generate_api_log(${name} API ${api} PREFIX ${prefix})
    endif()

    if (NOT arg_NO_MODULE_ENTRY)
        if (NOT DEFINED arg_INTERFACE_CLASS OR arg_INTERFACE_CLASS STREQUAL "")
            set(arg_INTERFACE_CLASS "ModuleInterface")
        endif()

        if (arg_NO_MODULE_INTERFACE)
            generate_module_entry(${name}
                API ${api}
                PREFIX ${prefix}
                NULL_INTERFACE
            )
        else()
            generate_module_entry(${name}
                API ${api}
                PREFIX ${prefix}
                INTERFACE_CLASS ${arg_INTERFACE_CLASS}
            )
        endif()
    endif()

    set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")
    if (EXISTS ${include_dir})
        target_include_directories(${name} PUBLIC ${include_dir})
    endif()

    set_target_properties(${name} PROPERTIES 
        FOLDER module
        OUTPUT_NAME "${prefix}${BUILD_PREFIX}.${api}"
    )
endfunction()