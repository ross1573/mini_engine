include(mini_define)
include(mini_option)
include(mini_module)

set_output_directory(${BUILD_TARGET_ARCH})

function (no_arg_test name)
    add_executable(test.${name} ${name}.cpp)
    add_test(NAME ${name} COMMAND $<TARGET_FILE:test.${name}>)

    generate_define_header(test.${name})
    
    target_link_libraries(test.${name} PRIVATE mini.test)
    set_target_properties(test.${name} PROPERTIES
        FOLDER test
        OUTPUT_NAME $<IF:$<CONFIG:Debug>,testd,test>.${name}
    )

    if (NOT WIN32)
        set_target_properties(test.${name} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${ENGINE_LIBRARY_DIR}/test
            LIBRARY_OUTPUT_DIRECTORY ${ENGINE_LIBRARY_DIR}/test
            RUNTIME_OUTPUT_DIRECTORY ${ENGINE_RUNTIME_DIR}/test
        )
    endif()

    build_source_tree(test.${name})
endfunction()

add_module(mini.test SHARED NO_MODULE_INTERFACE)

module_sources(mini.test
PUBLIC
    test_macro.h

PUBLIC
    FILE_SET test TYPE CXX_MODULES
    FILES
        test.cxx
)

target_include_directories(mini.test PUBLIC ./)
target_link_libraries(mini.test PUBLIC mini.core)

add_subdirectory(utility)
add_subdirectory(memory)
add_subdirectory(string)
add_subdirectory(container)
add_subdirectory(math)
add_subdirectory(chrono)