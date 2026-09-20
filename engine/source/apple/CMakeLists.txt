add_module(mini.apple STATIC)

enable_language(OBJCXX)

target_sources(mini.apple
PUBLIC
    FILE_SET apple TYPE CXX_MODULES
    FILES
        apple.cxx
        apple_assert.cxx
        apple_foundation.cxx
        apple_string.cxx
        apple_window.cxx
        apple_shared_ptr.cxx
)

target_link_libraries(mini.apple
PUBLIC
    mini.core
    mini.platform
    metal.cpp
)

target_include_directories(mini.apple
PUBLIC
    include/
)

add_subdirectory(macos)