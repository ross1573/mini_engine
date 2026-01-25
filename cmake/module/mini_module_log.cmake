include(module/mini_module_api)

function (generate_module_log target)
    set(args PREFIX API INTERFACE IMPORT)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "${args}" "")
    
    generate_api_name(${target} PREFIX ${arg_PREFIX} API ${arg_API})

    snake_to_camel_case(${api})
    set(file_path "${CMAKE_CURRENT_BINARY_DIR}/${target}.generated")
    set(file_name "${target}.log.generated.cxx")

    configure_file(
        "${ENGINE_PROJECT_DIR}/cmake/template/logger.cxx"
        "${file_path}/${file_name}"
        @ONLY
        NEWLINE_STYLE LF
    )

    target_sources(${target} PUBLIC
        FILE_SET generated_log TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES "${file_path}/${file_name}"
    )
endfunction()