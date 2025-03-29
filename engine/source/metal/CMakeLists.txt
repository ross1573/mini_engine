add_module(mini.metal SHARED)

module_sources(mini.metal
PUBLIC
    FILE_SET metal TYPE CXX_MODULES
    FILES

PRIVATE
)

target_link_libraries(mini.metal
PUBLIC
    mini.core
    mini.graphics

PRIVATE
    mini.macos
    mini.engine
)