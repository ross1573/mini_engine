include(mini_module_util)
include(GenerateExportHeader)

function (module_sources name)
    target_sources(${ARGV})
    build_source_tree(${name})
endfunction()

function (add_module name)
    generate_api_name(${name})
    
    if (${ARGC} GREATER 1)
        list(GET ARGN 0 first_arg)
        string(TOUPPER ${first_arg} type)
    endif()

    if (type MATCHES "STATIC|SHARED")
        list(POP_FRONT ARGN)
        math(EXPR ${ARGC} "${ARGC}-1")
    endif()

    add_library(${name} ${type})

    generate_export_header(${name}
        BASE_NAME "${api_upper}"
        EXPORT_FILE_NAME "${api}.generated.h"
        EXPORT_MACRO_NAME "${api_upper}_API"
        NO_EXPORT_MACRO_NAME "${api_upper}_HIDDEN"
        DEPRECATED_MACRO_NAME "${api_upper}_DEPRECATED"
    )

    set(export_header "${CMAKE_CURRENT_BINARY_DIR}/${api}.generated.h")
    target_sources(${name} PRIVATE ${export_header})
    target_precompile_headers(${name} PRIVATE ${export_header})

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