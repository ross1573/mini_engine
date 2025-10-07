include(GenerateExportHeader)

macro (generate_api_name name)
    string(REGEX REPLACE "mini[._]" "" api ${name})
    string(REPLACE "." "_" api ${api})
    string(TOUPPER ${api} api_upper)
endmacro()

function (generate_module_entry name)
    generate_api_name(${name})
    snake_to_camel_case(${api})
    
    string(CONCAT class "mini::" ${api} "::ModuleInterface")
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

    target_sources(${name} PRIVATE ${file_path})
endfunction()