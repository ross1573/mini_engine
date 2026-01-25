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