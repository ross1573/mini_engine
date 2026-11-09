include(external/NuGet)
include(mini_util)

add_module(mini.d3d12
    INTERFACE mini::D3D12
)

target_sources(mini.d3d12
PUBLIC
    FILE_SET d3d12 TYPE CXX_MODULES
    FILES
        d3d12.cxx
        d3d12_assert.cxx
        d3d12_common.cxx
        d3d12_event.cxx
        d3d12_resource.cxx
        d3d12_command_buffer.cxx
        d3d12_command_queue.cxx
        d3d12_descriptor.cxx
        d3d12_device.cxx
        d3d12_renderer.cxx
        d3d12_swap_chain.cxx

PUBLIC
    include/d3d12_include.h
    include/d3d12_uuid.h

PRIVATE
    impl/d3d12.cpp
    impl/d3d12_event.cpp
    impl/d3d12_resource.cpp
    impl/d3d12_command_buffer.cpp
    impl/d3d12_command_queue.cpp
    impl/d3d12_descriptor.cpp
    impl/d3d12_device.cpp
    impl/d3d12_renderer.cpp
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

target_precompile_headers(mini.d3d12
PUBLIC
    include/d3d12_include.h
    include/d3d12_uuid.h
)

add_nuget_package(D3D12
    PACKAGE Microsoft.Direct3D.D3D12
    VERSION 1.620.1
)

if (ARM64)
    set(D3D12_ARCH_DIR "arm64")
elseif (X86_64)
    set(D3D12_ARCH_DIR "x64")
endif()

target_include_directories(mini.d3d12 SYSTEM BEFORE
PRIVATE
    "${D3D12_DIR}/native/include"
)

get_output_directory(output_dir RUNTIME)
add_custom_command(TARGET mini.d3d12 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${D3D12_DIR}/native/bin/${D3D12_ARCH_DIR}/D3D12Core.dll"
        "$<$<CONFIG:Debug>:${D3D12_DIR}/native/bin/${D3D12_ARCH_DIR}/D3D12Core.pdb>"
        "$<$<CONFIG:Debug>:${D3D12_DIR}/native/bin/${D3D12_ARCH_DIR}/d3d12SDKLayers.dll>"
        "$<$<CONFIG:Debug>:${D3D12_DIR}/native/bin/${D3D12_ARCH_DIR}/d3d12SDKLayers.pdb>"
        "${output_dir}"
    COMMAND_EXPAND_LISTS
)