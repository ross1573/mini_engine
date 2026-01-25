include (module/mini_module_api)

function (generate_static_init target)
    set(args PREFIX API INITIALIZE CLEANUP)
    cmake_parse_arguments(PARSE_ARGV 1 arg "${options}" "${args}" "")

    generate_api_name(${target} API ${arg_API} PREFIX ${arg_PREFIX})

    if (NOT DEFINED args_INITIALIZE OR arg_INITIALIZE STREQUAL "")
        set(args_INITIALIZE "StaticInitialize")
    endif()
    if (NOT DEFINED args_CLEANUP OR args_CLEANUP STREQUAL "")
        set(args_CLEANUP "StaticCleanup")
    endif()

    get_property(static_init GLOBAL PROPERTY STATIC_INIT)
    get_property(static_cleanup GLOBAL PROPERTY STATIC_CLEANUP)

    # define static init list
    if (NOT DEFINED static_init)
        set(static_init "")
    endif()
    if (NOT DEFINED static_cleanup)
        set(static_cleanup "")
    endif()

    # global module section where declarations are written
    if (NOT static_init STREQUAL "" OR NOT static_cleanup STREQUAL "")
        list(APPEND contents "module\;\n\n")
        foreach (init ${static_init})
            list(APPEND contents "extern \"C\" void ${init}()\;\n")
        endforeach()
        foreach (cleanup ${static_cleanup})
            list(APPEND contents "void ${cleanup}()\;\n")
        endforeach()
        list(APPEND contents "\n")
    endif()

    # start module and namespace
    string(REPLACE "." "::" namespace ${target})
    list(APPEND contents "export module ${target}:static_init\;\n\n")
    list(APPEND contents "namespace ${namespace} {\n\n")
    
    # write static initializer function
    list(APPEND contents "void ${args_INITIALIZE}()\n{\n")
    foreach (init ${static_init})
        list(APPEND contents "    ${init}()\;\n")
    endforeach()
    list(APPEND contents "}\n\n")

    # write static cleanup function
    list(APPEND contents "void ${args_CLEANUP}()\n{\n")
    foreach (cleanup ${static_cleanup})
        list(APPEND contents "    ${cleanup}()\;\n")
    endforeach()
    list(APPEND contents "}\n\n} // namespace ${namespace}\n")

    set(file_path "${CMAKE_CURRENT_BINARY_DIR}/${target}.generated")
    set(file_name "${target}.init.generated.cxx")
    file(WRITE "${file_path}/${file_name}" ${contents})

    target_sources(${target} PUBLIC
        FILE_SET generated_init TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES "${file_path}/${file_name}"
    )
endfunction()