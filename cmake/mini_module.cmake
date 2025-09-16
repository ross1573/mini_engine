include(build_source_tree)

macro (include_static name)
    set(BUILD_SHARED_LIBS FALSE)
    include(${name})
    set(BUILD_SHARED_LIBS TRUE)
endmacro()

macro (include_shared name)
    set(BUILD_SHARED_LIBS TRUE)
    include(${name})
    set(BUILD_SHARED_LIBS FALSE)
endmacro()

function (module_sources name)
    target_sources(${ARGV})
    build_source_tree(${name})
endfunction()

function (add_module name)
    string(REGEX REPLACE "mini[._]" "" api ${name})
    string(REPLACE "." "_" api ${api})
    string(REPLACE "." "_" header ${name})

    set(out_name ${BUILD_MODULE_PREFIX}.${api})
    set(api ${api}_API)
    set(header "${header}.generated.h")
    string(TOUPPER ${api} api)
    
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
        EXPORT_MACRO_NAME ${api}
        EXPORT_FILE_NAME ${header}
    )

    target_precompile_headers(${name}
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/${header}
    )

    target_include_directories(${name}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/public

    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/internal
    )

    set_target_properties(${name} PROPERTIES 
        FOLDER module
        OUTPUT_NAME ${out_name}
    )

    if (${ARGC} GREATER 1)
        module_sources(${name} ${ARGN})
    endif()
endfunction()