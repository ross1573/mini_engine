include(mini_module_util)

function (add_module name)    
    if (${ARGC} GREATER 1)
        list(GET ARGN 0 first_arg)
        string(TOUPPER ${first_arg} type)
    endif()

    if (type MATCHES "STATIC|SHARED|MODULE|INTERFACE")
        list(POP_FRONT ARGN)
        math(EXPR ${ARGC} "${ARGC}-1")
    endif()

    add_library(${name} ${type})

    generate_api_name(${name})
    if (NOT NO_API_HEADER)
        generate_api_header(${name})
    endif()
    if (NOT NO_API_LOG)
        generate_api_log(${name})
    endif()

    target_include_directories(${name}
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/internal"
    )

    set_target_properties(${name} PROPERTIES 
        FOLDER module
        OUTPUT_NAME "${BUILD_MODULE_PREFIX}.${api}"
    )

    if (${ARGC} GREATER 1)
        module_sources(${name} ${ARGN})
    endif()
endfunction()