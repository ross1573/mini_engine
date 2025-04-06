add_module(mini.metal SHARED)

enable_language(OBJCXX)

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
    impl/metal_layer.mm
)

target_include_directories(mini.metal
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/interface/
)

target_link_libraries(mini.metal
PUBLIC
    mini.core
    mini.graphics

PRIVATE
    mini.platform
    mini.engine
    mini.apple
    mini.macos
    metal.cpp
    "-framework Foundation"
    "-framework QuartzCore"
)