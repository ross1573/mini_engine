include(external/MetalCpp)

add_library(metal.cpp STATIC metal_cpp.cpp)

target_include_directories(metal.cpp
SYSTEM PUBLIC
    ${metal_cpp_SOURCE_DIR}
)

target_link_libraries(metal.cpp
PRIVATE
    "-framework Foundation"
    "-framework QuartzCore"
    "-framework Metal"
    "-framework MetalFX"
)