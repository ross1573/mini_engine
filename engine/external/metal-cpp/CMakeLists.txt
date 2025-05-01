add_library(metal.cpp STATIC metal-cpp.cpp)

target_include_directories(metal.cpp
SYSTEM PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(metal.cpp
PRIVATE
    "-framework Foundation"
    "-framework QuartzCore"
    "-framework Metal"
    "-framework MetalFX"
)