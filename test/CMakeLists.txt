include(mini_define)
include(mini_option)
include(build_source_tree)

function (no_arg_test name)
    add_executable(test.${name} ${name}.cpp)
    add_test(NAME ${name} COMMAND $<TARGET_FILE:test.${name}>)
    target_link_libraries(test.${name} PRIVATE mini.test)
    set_target_properties(test.${name} PROPERTIES 
        FOLDER test
        OUTPUT_NAME $<IF:$<CONFIG:Debug>,testd,test>.${name}
    )
    build_source_tree(test.${name})
endfunction()

add_library(mini.test)

target_sources(mini.test
PUBLIC
    test_macro.h

PUBLIC
FILE_SET test_private TYPE CXX_MODULES
FILES
    test.cxx
)

target_include_directories(mini.test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mini.test PUBLIC mini.core)
set_target_properties(mini.test PROPERTIES 
    FOLDER test
    OUTPUT_NAME $<IF:$<CONFIG:Debug>,minid,mini>.test
)
build_source_tree(mini.test)

add_subdirectory(container)
add_subdirectory(memory)