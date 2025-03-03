add_module(mini.platform SHARED)

target_sources(mini.platform
PUBLIC
    FILE_SET platform TYPE CXX_MODULES
    FILES
        platform.cxx

PRIVATE
    platform.cpp

PRIVATE
    FILE_SET platform_private TYPE CXX_MODULES
    FILES
        handle.cxx
        window.cxx
)

target_link_libraries(mini.platform 
PUBLIC
    mini.core

PRIVATE 
    mini.graphics
)