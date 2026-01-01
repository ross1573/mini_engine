add_module(mini.windows NO_MODULE_INTERFACE)

target_sources(mini.windows
PUBLIC
    FILE_SET windows TYPE CXX_MODULES
    FILES
        windows.cxx
        win_shared_ptr.cxx
        win_handle.cxx
        win_window.cxx

PRIVATE
    win_include.h
    impl/win_handle.cpp
    impl/win_window.cpp
)

target_link_libraries(mini.windows 
PUBLIC
    mini.core
    mini.platform

PRIVATE
    mini.graphics
    mini.engine
)

target_include_directories(mini.windows
PRIVATE
    ${ENGINE_PROJECT_DIR}/engine/platform/windows
)

target_precompile_headers(mini.windows
PRIVATE
    win_include.h
)

build_source_tree(mini.windows)