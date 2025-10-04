add_module(mini.platform)

module_sources(mini.platform
PUBLIC
    FILE_SET platform TYPE CXX_MODULES
    FILES
        platform.cxx
        handle.cxx
        window.cxx
        module.cxx

PRIVATE
    impl/platform.cpp
)

target_link_libraries(mini.platform 
PUBLIC
    mini.core

PRIVATE 
    mini.graphics
)