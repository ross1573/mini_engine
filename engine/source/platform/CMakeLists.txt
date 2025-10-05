add_module(mini.platform)

module_sources(mini.platform
PUBLIC
    FILE_SET platform TYPE CXX_MODULES
    FILES
        platform.cxx
        module_loader.cxx
        handle.cxx
        window.cxx

PRIVATE
    impl/platform.cpp
    impl/module_loader.cpp
)

target_link_libraries(mini.platform 
PUBLIC
    mini.core

PRIVATE 
    mini.graphics
)