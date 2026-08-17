include(mini_define)
include(mini_option)
include(mini_module)

set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

set_output_directory(test)
mini_compile_options()
mini_link_options()

function (no_arg_test name)
    set(target test.${name})

    add_executable(${target} ${name}.cpp)
    add_test(NAME ${name} COMMAND $<TARGET_FILE:test.${name}>)

    generate_define_header(${target})
    
    target_link_libraries(${target} PRIVATE mini.test)
    set_target_properties(${target} PROPERTIES
        FOLDER test
        OUTPUT_NAME ${target}${MODULE_OUTPUT_SUFFIX}
    )

    build_source_tree(${target})
endfunction()

add_module(mini.test SHARED NO_MODULE_ENTRY)

target_sources(mini.test
PUBLIC
    include/test_macro.h

PUBLIC
    FILE_SET test TYPE CXX_MODULES
    FILES
        test.cxx
)

target_include_directories(mini.test PUBLIC include)
target_link_libraries(mini.test PUBLIC mini.core)

# TODO: address sanitizer hangs on some situations. 
#       disabling it until finding out what is locking it
# if (MSVC)
#     target_link_options(mini.test PUBLIC $<$<BOOL:${X86}>:-fsanitize=address>)
# else()
#     target_link_options(mini.test PUBLIC -fsanitize=address)
# endif()

add_subdirectory(bit)
add_subdirectory(memory)
add_subdirectory(math)
add_subdirectory(string)
add_subdirectory(container)
add_subdirectory(chrono)
add_subdirectory(concurrency)