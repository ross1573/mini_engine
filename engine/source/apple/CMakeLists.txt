add_library(mini.apple INTERFACE)

target_include_directories(mini.apple
INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/public
)

add_subdirectory(macos)