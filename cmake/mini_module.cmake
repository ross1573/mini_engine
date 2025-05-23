include(build_source_tree)

function (module_sources name)
    target_sources(${ARGV})
    build_source_tree(${name})
endfunction()

function (add_module name type)
    string(REGEX REPLACE "mini[._]" "" api ${name})
    string(REPLACE "." "_" api ${api})
    string(REPLACE "." "_" header ${name})

    set(out_name ${ENGINE_MODULE_PREFIX}.${api})
    set(api ${api}_API)
    set(header "${header}.generated.h")
    string(TOUPPER ${api} api)

    add_library(${name} ${type})

    generate_export_header(${name}
        EXPORT_MACRO_NAME ${api}
        EXPORT_FILE_NAME ${header}
    )

    target_precompile_headers(${name}
    PUBLIC
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

    if (${ARGC} GREATER 2)
        module_sources(${name} ${ARGN})
    endif()
endfunction()