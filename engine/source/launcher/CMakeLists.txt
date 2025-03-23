if (WIN32)
    add_executable(mini.launcher WIN32 win_main.cpp)
elseif (APPLE)
    add_executable(mini.launcher macos_main.cpp)
else()
    message(FATAL_ERROR "unable to build on unknown platform")
endif()

target_sources(mini.launcher
PUBLIC 
FILE_SET launcher TYPE CXX_MODULES
FILES
    launcher.cxx
)

target_link_libraries(mini.launcher
PRIVATE 
    mini.core
    mini.engine
    mini.platform

    $<$<PLATFORM_ID:Windows>:mini.windows>
    # $<$<PLATFORM_ID:Darwin>:mini.macos>
)

set_target_properties(mini.launcher
PROPERTIES
    FOLDER module
    OUTPUT_NAME ${PROJECT_NAME}$<$<CONFIG:Debug>:Debug>
)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set_property(DIRECTORY ${CMAKE_BINARY_DIR} PROPERTY VS_STARTUP_PROJECT mini.launcher)
endif()

build_source_tree(mini.launcher)