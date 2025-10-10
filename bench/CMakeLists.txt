include(mini_define)
include(mini_option)
include(mini_module)
include_static(GoogleBenchmark)

function (no_arg_benchmark name)
    add_executable(benchmark.${name} ${name}.cpp)

    generate_define_header(benchmark.${name})
    
    target_link_libraries(benchmark.${name} PRIVATE mini.benchmark)
    set_target_properties(benchmark.${name} PROPERTIES
        FOLDER bench
        OUTPUT_NAME $<IF:$<CONFIG:Debug>,benchd,bench>.${name}
    )

    if (NOT WIN32)
        set_target_properties(benchmark.${name} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${ENGINE_LIBRARY_DIR}/bench
            LIBRARY_OUTPUT_DIRECTORY ${ENGINE_LIBRARY_DIR}/bench
            RUNTIME_OUTPUT_DIRECTORY ${ENGINE_RUNTIME_DIR}/bench
        )
    endif()

    set(OPT_RESULT_PATH ${ENGINE_PROJECT_DIR}/bin/bench/benchmark.${name}.yaml)
    target_compile_options(benchmark.${name}
    PRIVATE
        $<$<CXX_COMPILER_ID:Clang>:-fsave-optimization-record> 
        $<$<CXX_COMPILER_ID:Clang>:-foptimization-record-file=${OPT_RESULT_PATH}>
    )

    build_source_tree(benchmark.${name})
endfunction()

add_module(mini.benchmark STATIC)

module_sources(mini.benchmark
PUBLIC
    FILE_SET benchmark TYPE CXX_MODULES
    FILES
        benchmark.cxx
)

target_link_libraries(mini.benchmark 
PUBLIC 
    mini.core
    benchmark::benchmark
)

add_subdirectory(container)