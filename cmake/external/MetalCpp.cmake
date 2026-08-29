include(FetchContent)

FetchContent_Declare(metal_cpp
    GIT_REPOSITORY https://github.com/apple/metal-cpp.git
    GIT_TAG release/metal-cpp_macOS26.4_iOS26.4
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)

FetchContent_MakeAvailable(metal_cpp)