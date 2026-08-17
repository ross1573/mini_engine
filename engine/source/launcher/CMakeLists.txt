include(module/mini_module_define)
include(module/mini_module_static)
include(module/mini_module_bundle)

if (WIN32)
    add_executable(mini.launcher WIN32 windows/win_main.cpp)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    enable_language(OBJCXX)

    add_executable(mini.launcher MACOSX_BUNDLE 
        macos/macos_main.cpp
        macos/macos_init.mm
    )

    target_link_libraries(mini.launcher PRIVATE "-framework AppKit")
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
generate_module_log(mini.launcher)
generate_static_init(mini.launcher
    INITIALIZE StaticInitialize
    CLEANUP StaticCleanup
)

get_modules(static_modules STATIC)
set(modules
    mini.core
    mini.engine
)

list(APPEND modules ${static_modules})
list(REMOVE_DUPLICATES modules)
target_link_libraries(mini.launcher PRIVATE ${modules})

relocate_executable(mini.launcher)
set_target_properties(mini.launcher
PROPERTIES
    FOLDER module
    OUTPUT_NAME "${BUNDLE_OUTPUT_NAME}"
)

set_startup_project(mini.launcher)
build_source_tree(mini.launcher)