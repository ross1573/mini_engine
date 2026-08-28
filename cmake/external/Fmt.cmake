include (FetchContent)

set(FMT_DOC OFF)
set(FMT_INSTALL OFF)
set(FMT_TEST OFF)
set(FMT_FUZZ OFF)
set(FMT_CUDA_TEST OFF)
set(FMT_OS OFF)

set(FMT_MODULE ON)
set(FMT_SYSTEM_HEADERS ON)
set(FMT_UNICODE ON)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 12.2.0
)
FetchContent_MakeAvailable(fmt)

set_target_properties(fmt PROPERTIES FOLDER external)
set_target_properties(fmt-c PROPERTIES FOLDER external)
set_target_properties(fmt-module PROPERTIES FOLDER external)

# quick workaround for disabling standard library format support
target_compile_definitions(fmt-module PRIVATE FMT_STD_H_)