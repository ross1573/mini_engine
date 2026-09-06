include(metal/Metal)

add_metal_library(mini.shader)

target_sources(mini.shader
PRIVATE
    shader.metal
)