if (MSVC)
    add_executable(mini.launcher WIN32
        win_main.cpp
        launcher.cxx   
    )
else()
    message(FATAL_ERROR "unable to build on unknown platform")
endif()

target_link_libraries(mini.launcher
PRIVATE 
    mini.core
    mini.engine
    mini.windows
)

set_target_properties(mini.launcher PROPERTIES FOLDER modules)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set_property(DIRECTORY ${CMAKE_BINARY_DIR} PROPERTY VS_STARTUP_PROJECT mini.launcher)
endif()

build_source_tree(mini.launcher)