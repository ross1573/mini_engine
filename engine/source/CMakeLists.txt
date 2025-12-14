set(CMAKE_CXX_SCAN_FOR_MODULES TRUE)
set(CMAKE_MESSAGE_LOG_LEVEL STATUS)

include(mini_define)
include(mini_option)
include(mini_module)

set_output_directory(${BUILD_TARGET_ARCH})

add_subdirectory_analyzed(core)
add_subdirectory_analyzed(platform)
add_subdirectory_analyzed(graphics)

if (WIN32)
    add_subdirectory_analyzed(windows)
    add_subdirectory_analyzed(d3d12)
elseif (APPLE)
    add_subdirectory_analyzed(apple)
    add_subdirectory_analyzed(metal)
endif()

add_subdirectory_analyzed(engine)
add_subdirectory_analyzed(launcher)

exec_build_source_tree()