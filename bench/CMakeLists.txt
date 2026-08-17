include(mini_define)
include(mini_option)
include(mini_module)
include_shared(external/GoogleBenchmark)

set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

set_output_directory(bench)
mini_compile_options()
mini_link_options()

function (no_arg_benchmark name)
    set(target benchmark.${name})
    set(output bench.${name})

    add_executable(${target} ${name}.cpp)
    
    generate_define_header(${target})

    target_link_libraries(${target} PRIVATE mini.benchmark)
    set_target_properties(${target} PROPERTIES
        FOLDER bench
        OUTPUT_NAME ${output}${MODULE_OUTPUT_SUFFIX}
    )

    set(OPT_RESULT_PATH ${ENGINE_PROJECT_DIR}/bin/bench/${target}.yaml)
    target_compile_options(benchmark.${name}
    PRIVATE
        $<$<CXX_COMPILER_ID:Clang>:-fsave-optimization-record> 
        $<$<CXX_COMPILER_ID:Clang>:-foptimization-record-file=${OPT_RESULT_PATH}>
    )

    build_source_tree(${target})
endfunction()

add_module(mini.benchmark STATIC
    NO_MODULE_LOG
    NO_MODULE_ENTRY
)

target_sources(mini.benchmark
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

add_subdirectory(string)
add_subdirectory(concurrency)