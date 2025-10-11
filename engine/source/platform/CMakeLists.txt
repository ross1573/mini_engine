add_module(mini.platform NO_MODULE_INTERFACE)

module_sources(mini.platform
PUBLIC
    FILE_SET platform TYPE CXX_MODULES
    FILES
        platform.cxx
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