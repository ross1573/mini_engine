function(add_module name type)
    string(REGEX REPLACE "mini[._]" "" api ${name})
    string(REPLACE "." "_" api ${api})
    string(REPLACE "." "_" header ${name})

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
endfunction()