include(mini_define)
include(mini_option)
include(build_source_tree)
include(GoogleBenchmark)

function (no_arg_benchmark name)
    add_executable(benchmark.${name} ${name}.cpp)
    
    target_link_libraries(benchmark.${name} PRIVATE mini.benchmark)
    set_target_properties(benchmark.${name} PROPERTIES
        FOLDER bench
        OUTPUT_NAME $<IF:$<CONFIG:Debug>,benchd,bench>.${name}
        ARCHIVE_OUTPUT_DIRECTORY ${ENGINE_PROJECT_DIR}/lib/bench
        LIBRARY_OUTPUT_DIRECTORY ${ENGINE_PROJECT_DIR}/lib/bench
        RUNTIME_OUTPUT_DIRECTORY ${ENGINE_PROJECT_DIR}/bin/bench
    )

    set(OPT_RESULT_PATH ${ENGINE_PROJECT_DIR}/bin/bench/benchmark.${name}.yaml)
    target_compile_options(benchmark.${name}
    PRIVATE
        $<$<CXX_COMPILER_ID:Clang>:-fsave-optimization-record> 
        $<$<CXX_COMPILER_ID:Clang>:-foptimization-record-file=${OPT_RESULT_PATH}>
    )

    build_source_tree(benchmark.${name})
endfunction()

add_library(mini.benchmark STATIC)

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

set_target_properties(mini.benchmark
PROPERTIES
    FOLDER module
    OUTPUT_NAME $<IF:$<CONFIG:Debug>,minid,mini>.benchmark
)

build_source_tree(mini.benchmark)

add_subdirectory(container)