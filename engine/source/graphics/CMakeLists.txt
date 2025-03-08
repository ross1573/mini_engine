add_module(mini.graphics SHARED)

module_sources(mini.graphics
PUBLIC
    FILE_SET graphics TYPE CXX_MODULES
    FILES
        device.cxx
        graphics.cxx
        graphics_common.cxx
        render_context.cxx
        swap_chain.cxx

PRIVATE
    graphics.cpp
)

target_link_libraries(mini.graphics
PUBLIC 
    mini.core
)