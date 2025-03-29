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
    macos_handle.cpp
    macos_window.cpp
    cocoa/application.mm
    cocoa/application_delegate.mm
    cocoa/window.mm
    cocoa/window_delegate.mm
)

target_link_libraries(mini.macos
PUBLIC
    mini.core
    mini.platform

PRIVATE
    mini.graphics
    mini.engine
    metal_cpp
    "-framework Foundation"
    "-framework AppKit"
)

target_compile_definitions(mini.macos
PRIVATE
    NS_PRIVATE_IMPLEMENTATION
    CA_PRIVATE_IMPLEMENTATION
    MTL_PRIVATE_IMPLEMENTATION
)