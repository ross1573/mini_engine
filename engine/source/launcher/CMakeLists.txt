include(module/mini_module_define)
include(module/mini_module_static)

if (WIN32)
    add_executable(mini.launcher WIN32 windows/win_main.cpp)
elseif (APPLE)
    enable_language(OBJCXX)

    add_executable(mini.launcher MACOSX_BUNDLE 
        macos/macos_main.cpp
        macos/macos_init.mm
    )

    target_link_libraries(mini.launcher PRIVATE "-framework AppKit")
    set_target_properties(mini.launcher 
    PROPERTIES 
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME ${ENGINE_PROJECT_NAME}
        MACOSX_BUNDLE_BUNDLE_VERSION ${ENGINE_PROJECT_VERSION}
        MACOSX_BUNDLE_COPYRIGHT ${ENGINE_PROJECT_AUTHOR}
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.${ENGINE_PROJECT_AUTHOR}.${ENGINE_PROJECT_NAME}"
    )
else()
    message(FATAL_ERROR "unable to build on unknown platform: " ${CMAKE_SYSTEM_NAME})
endif()

target_sources(mini.launcher
PUBLIC 
    FILE_SET launcher TYPE CXX_MODULES
    FILES
        launcher.cxx
)

generate_define_header(mini.launcher)
generate_static_init(mini.launcher
    INITIALIZE StaticInitialize
    CLEANUP StaticCleanup
)

get_property(static_modules GLOBAL PROPERTY STATIC_MODULE)
set(modules
    mini.core
    mini.engine
)

list(APPEND modules ${static_modules})
list(REMOVE_DUPLICATES modules)
target_link_libraries(mini.launcher PRIVATE ${modules})

set_target_properties(mini.launcher
PROPERTIES
    FOLDER module
    OUTPUT_NAME ${PROJECT_NAME}$<$<CONFIG:Debug>:Debug>
)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set_property(DIRECTORY ${CMAKE_BINARY_DIR} PROPERTY VS_STARTUP_PROJECT mini.launcher)
endif()

build_source_tree(mini.launcher)