add_module(mini.graphics)

target_sources(mini.graphics
PUBLIC
    FILE_SET graphics TYPE CXX_MODULES
    FILES
        device.cxx
        graphics.cxx
        graphics_common.cxx
        render_context.cxx
        swap_chain.cxx

PRIVATE
    impl/graphics.cpp
)

target_link_libraries(mini.graphics
PUBLIC 
    mini.core
)

build_source_tree(mini.graphics)