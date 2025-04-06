add_module(mini.macos SHARED)

enable_language(OBJCXX)

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
)

target_link_libraries(mini.macos
PUBLIC
    mini.core
    mini.platform

PRIVATE
    mini.graphics
    mini.engine
    mini.apple
    metal.cpp
    "-framework Foundation"
    "-framework AppKit"
)

target_include_directories(mini.macos
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/interface
)