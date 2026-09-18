include(metal/Metal)

add_metal_library(mini.metal.shader)

target_sources(mini.metal.shader
PRIVATE
    shader.metal
)