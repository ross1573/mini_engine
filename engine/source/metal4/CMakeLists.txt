add_module(mini.metal4
    INTERFACE mini::Metal4
)

module_compile_definitions(mini.metal4
    SHADER_LIBRARY_SEARCH_PATH="Contents/Resources"
    SHADER_LIBRARY_EXTENSION=".metallib"
)

target_sources(mini.metal4
PUBLIC
    FILE_SET metal4 TYPE CXX_MODULES
    FILES
        metal4.cxx
        metal4_common.cxx
        metal4_resource.cxx
        metal4_buffer.cxx
        metal4_texture.cxx
        metal4_shader.cxx
        metal4_device.cxx
        metal4_swap_chain.cxx
        metal4_renderer.cxx
        metal4_compiler.cxx
        metal4_render_pass.cxx
        metal4_render_pipeline.cxx

PRIVATE
    impl/metal4.cpp
    impl/metal4_shader.cpp
    impl/metal4_device.cpp
    impl/metal4_swap_chain.cpp
    impl/metal4_renderer.cpp
    impl/metal4_compiler.cpp
    impl/metal4_render_pass.cpp
    impl/metal4_render_pipeline.cpp
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