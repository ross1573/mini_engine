add_module(mini.metal4
    INTERFACE mini::Metal4
)

target_sources(mini.metal4
PUBLIC
    FILE_SET metal4 TYPE CXX_MODULES
    FILES
        metal4.cxx
        metal4_device.cxx
        metal4_swap_chain.cxx
        metal4_renderer.cxx

PRIVATE
    impl/metal4.cpp
    impl/metal4_device.cpp
    impl/metal4_swap_chain.cpp
    impl/metal4_renderer.cpp
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