add_module(mini.platform
    INTERFACE mini::Platform
)

if (WIN32)
    set(native_module "mini.windows")
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(native_module "mini.macos")
else()
    message(FATAL_ERROR "unknown platform: " ${CMAKE_SYSTEM_NAME})
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
        handle.cxx
        window.cxx

PRIVATE
    impl/platform.cpp
)

target_link_libraries(mini.platform 
PUBLIC
    mini.core

PRIVATE 
    mini.graphics
)