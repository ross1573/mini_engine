add_module(mini.apple STATIC)

module_sources(mini.apple
PUBLIC
    FILE_SET apple TYPE CXX_MODULES
    FILES
        apple.cxx
        apple_window.cxx
)

target_link_libraries(mini.apple
PUBLIC
    mini.core
    mini.platform
)

target_include_directories(mini.apple
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/public
)

add_subdirectory(macos)