add_module(mini.apple STATIC)

enable_language(OBJCXX)

target_sources(mini.apple
PUBLIC
    include/appkit.h
    include/foundation.h

PUBLIC
    FILE_SET apple TYPE CXX_MODULES
    FILES
        apple.cxx
        apple_window.cxx
        apple_shared_ptr.cxx
)

target_precompile_headers(mini.apple
PUBLIC
    include/appkit.h
    include/foundation.h
    <Foundation/Foundation.hpp>
    <QuartzCore/QuartzCore.hpp>
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