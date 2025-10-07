add_module(mini.d3d12)

include(windows_header_define)
include(${ENGINE_EXTERNAL_DIR}/d3d12/d3d12.cmake)

target_precompile_headers(mini.d3d12
PRIVATE
    <d3d12.h>
    <dxgi1_6.h>
    $<$<CONFIG:Debug>:<dxgidebug.h$<ANGLE-R>>
)

module_sources(mini.d3d12
PUBLIC
    FILE_SET d3d12 TYPE CXX_MODULES
    FILES
        d3d12.cxx
        d3d12_command_queue.cxx
        d3d12_descriptor.cxx
        d3d12_device.cxx
        d3d12_render_context.cxx
        d3d12_swap_chain.cxx

PRIVATE
    impl/d3d12_command_queue.cpp
    impl/d3d12_descriptor.cpp
    impl/d3d12_device.cpp
    impl/d3d12_render_context.cpp
    impl/d3d12_swap_chain.cpp
)

target_link_libraries(mini.d3d12
PUBLIC
    mini.core
    mini.graphics

PRIVATE
    d3d12.lib
    dxgi.lib
    dxguid.lib
    d3dcompiler.lib
    mini.windows
    mini.engine
)

copy_d3d12(mini.d3d12)
generate_module_entry(mini.d3d12)