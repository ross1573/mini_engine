add_module(mini.metal4
    INTERFACE mini::Metal4
)

target_sources(mini.metal4
PUBLIC
    FILE_SET metal4 TYPE CXX_MODULES
    FILES
        metal4.cxx
        metal4_resource.cxx
        metal4_buffer.cxx
        metal4_common.cxx
        metal4_device.cxx
        metal4_swap_chain.cxx
        metal4_renderer.cxx
        metal4_render_pass.cxx

PRIVATE
    impl/metal4.cpp
    impl/metal4_device.cpp
    impl/metal4_swap_chain.cpp
    impl/metal4_renderer.cpp
    impl/metal4_render_pass.cpp
)

target_precompile_headers(mini.metal4
PRIVATE
    <Metal/Metal.hpp>
    <MetalFX/MetalFX.hpp>
    <Foundation/Foundation.hpp>
    <QuartzCore/QuartzCore.hpp>
)

target_link_libraries(mini.metal4
PUBLIC
    mini.graphics

PRIVATE
    mini.engine
    mini.apple
    metal.cpp
    "-framework Foundation"
    "-framework QuartzCore"
)