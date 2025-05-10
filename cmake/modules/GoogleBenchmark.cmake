include(FetchContent)

# save binaries on external folder
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ENGINE_PROJECT_DIR}/lib/external")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ENGINE_PROJECT_DIR}/lib/external")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ENGINE_PROJECT_DIR}/bin/external")

set(BENCHMARK_ENABLE_WERROR OFF)
set(BENCHMARK_ENABLE_TESTING OFF) 
set(BENCHMARK_ENABLE_GTEST_TESTS OFF)

FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.3
)
FetchContent_MakeAvailable(googlebenchmark)

set_target_properties(benchmark 
PROPERTIES
    FOLDER external
)

set_target_properties(benchmark_main
PROPERTIES
    FOLDER external
)
