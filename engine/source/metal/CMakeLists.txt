add_module(mini.metal SHARED)

module_sources(mini.metal
PUBLIC
    FILE_SET metal TYPE CXX_MODULES
    FILES
        metal.cxx
        metal_device.cxx
        metal_swap_chain.cxx
        metal_render_context.cxx

PRIVATE
    impl/metal_device.cpp
    impl/metal_swap_chain.cpp
    impl/metal_render_context.cpp
)

target_link_libraries(mini.metal
PUBLIC
    mini.graphics

PRIVATE
    mini.engine
    mini.apple
    metal.cpp
    "-framework Foundation"
    "-framework QuartzCore"
)