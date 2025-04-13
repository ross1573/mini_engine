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
    metal.cpp
)

add_subdirectory(macos)