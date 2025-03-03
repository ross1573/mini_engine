add_module(mini.d3d12 SHARED)

include(windows_header_define)

target_precompile_headers(mini.d3d12
PRIVATE
    <d3d12.h>
    <dxgi1_6.h>
    $<$<CONFIG:Debug>:<dxgidebug.h$<ANGLE-R>>
)

target_sources(mini.d3d12
PUBLIC
    FILE_SET d3d12 TYPE CXX_MODULES
    FILES
        d3d12.cxx

PRIVATE
    d3d12_command_queue.cpp
    d3d12_descriptor.cpp
    d3d12_device.cpp
    d3d12_render_context.cpp
    d3d12_swap_chain.cpp

PRIVATE
    FILE_SET d3d12_private TYPE CXX_MODULES
    FILES
        d3d12_command_queue.cxx
        d3d12_descriptor.cxx
        d3d12_device.cxx
        d3d12_render_context.cxx
        d3d12_swap_chain.cxx
)

target_link_libraries(mini.d3d12
PUBLIC
    mini.core
    mini.graphics

PRIVATE
    mini.windows
    mini.engine
)