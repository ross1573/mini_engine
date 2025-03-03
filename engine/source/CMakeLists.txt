set(CMAKE_CXX_SCAN_FOR_MODULES ON)

include(mini_module)
include(mini_define)
include(mini_option)

include_directories(${ENGINE_EXTERNAL_DIR})
include_directories(${ENGINE_SOURCE_DIR})

link_directories(${ENGINE_EXTERNAL_DIR})

add_subdirectory(core)
add_subdirectory(platform)
add_subdirectory(graphics)

if (WIN32)
    add_subdirectory(windows)
    add_subdirectory(d3d12)
endif()

add_subdirectory(engine)
add_subdirectory(launcher)