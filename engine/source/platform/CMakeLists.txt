add_module(mini.platform
    INTERFACE mini::Platform
)

if (WINDOWS)
    set(native_module "mini.windows")
elseif (MACOS)
    set(native_module "mini.macos")
endif()

module_compile_definitions(mini.platform
    MODULE_NATIVE="${native_module}"
)

target_sources(mini.platform
PUBLIC
    FILE_SET platform TYPE CXX_MODULES
    FILES
        platform.cxx
        platform_interface.cxx
        platform_handle.cxx
        platform_window.cxx

PRIVATE
    impl/platform.cpp
)

target_link_libraries(mini.platform 
PUBLIC
    mini.core

PRIVATE 
    mini.graphics
)