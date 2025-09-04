add_module(mini.apple STATIC)

module_sources(mini.apple
PUBLIC
    FILE_SET apple TYPE CXX_MODULES
    FILES
        apple.cxx
        apple_window.cxx
        apple_shared_ptr.cxx
)

target_precompile_headers(mini.apple
PUBLIC
    public/appkit.h
    public/foundation.h
)

target_link_libraries(mini.apple
PUBLIC
    mini.core
    mini.platform
    metal.cpp
)

add_subdirectory(macos)