include(module/mini_module_api)

function (generate_module_entry target)
    set(args PREFIX API INTERFACE IMPORT)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "${args}" "")
    
    generate_api_name(${target} PREFIX ${arg_PREFIX} API ${arg_API})

    if (NOT DEFINED arg_INTERFACE OR arg_INTERFACE STREQUAL "")
        set(class "DefaultInterface")
    else()
        set(class ${arg_INTERFACE})
    endif()

    if (NOT DEFINED arg_IMPORT OR arg_IMPORT STREQUAL "")
        set(import "mini.core")
    else()
        set(import ${arg_IMPORT})
    endif()
    
    set(file_path "${CMAKE_CURRENT_BINARY_DIR}/${target}.generated")
    set(file_name "${target}.entry.generated.cpp")

    configure_file(
        "${ENGINE_PROJECT_DIR}/cmake/template/module_entry.cpp"
        "${file_path}/${file_name}"
        @ONLY
        NEWLINE_STYLE LF
    )

    # cache build type specific initializers
    # these initializers will be used on code generated
    get_target_property(target_type ${target} TYPE)
    if (target_type STREQUAL "STATIC_LIBRARY")
        set_property(GLOBAL APPEND PROPERTY STATIC_MODULE ${target})
        set_property(GLOBAL APPEND PROPERTY STATIC_INIT "__${api_full}_start_module")
    elseif (NOT target_type STREQUAL "SHARED_LIBRARY")
        message(FATAL_ERROR "unsupported build type: " ${target_type})
    endif()

    target_sources(${target} PRIVATE "${file_path}/${file_name}")
    set_source_files_properties("${file_path}/${file_name}" PROPERTIES GENERATED ON)
endfunction()