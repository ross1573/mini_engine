include(FetchContent)

# save output on external folder
set_output_directory(external)

set(BENCHMARK_ENABLE_TESTING OFF)
set(BENCHMARK_ENABLE_EXCEPTIONS OFF)
set(BENCHMARK_ENABLE_LTO OFF)
set(BENCHMARK_ENABLE_WERROR OFF)
set(BENCHMARK_ENABLE_INSTALL OFF)
set(BENCHMARK_ENABLE_DOXYGEN OFF)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF)

set(BENCHMARK_INSTALL_DOCS OFF)

set(BENCHMARK_USE_BUNDLED_GTEST OFF)

FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.4
)
FetchContent_MakeAvailable(googlebenchmark)

set_target_properties(benchmark PROPERTIES FOLDER external)
set_target_properties(benchmark_main PROPERTIES FOLDER external)

# restore output folder
set_output_directory(${BUILD_TARGET_ARCH})
