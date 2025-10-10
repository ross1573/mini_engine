add_module(mini.macos)

module_sources(mini.macos
PUBLIC
    FILE_SET macos TYPE CXX_MODULES
    FILES
        macos.cxx
        macos_handle.cxx
        macos_window.cxx

PRIVATE
    impl/macos_handle.cpp
    impl/macos_window.cpp
    impl/cocoa_application.mm
    impl/cocoa_window.mm
    impl/cocoa_view.mm
)

target_precompile_headers(mini.macos
PRIVATE
    internal/cocoa_application.h
    internal/cocoa_window.h
)

target_link_libraries(mini.macos
PRIVATE
    mini.graphics
    mini.engine
    mini.apple
    "-framework Foundation"
    "-framework AppKit"
)

target_include_directories(mini.macos
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/internal
)