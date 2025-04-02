set(CMAKE_CXX_SCAN_FOR_MODULES TRUE)
set(CMAKE_MESSAGE_LOG_LEVEL STATUS)

include(mini_module)
include(mini_define)
include(mini_option)

add_subdirectory(core)
add_subdirectory(platform)
add_subdirectory(graphics)

if (WIN32)
    add_subdirectory(windows)
    add_subdirectory(d3d12)
elseif (APPLE)
    add_subdirectory(macos)
    add_subdirectory(metal)
endif()

add_subdirectory(engine)
add_subdirectory(launcher)