add_module(mini.graphics
    INTERFACE mini::Graphics
)

target_sources(mini.graphics
PUBLIC
    FILE_SET graphics TYPE CXX_MODULES
    FILES
        graphics.cxx
        graphics_interface.cxx
        graphics_common.cxx
        device.cxx
        render_context.cxx
        swap_chain.cxx

PRIVATE
    impl/graphics.cpp
)

target_link_libraries(mini.graphics
PUBLIC 
    mini.core
)