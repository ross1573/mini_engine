add_module(mini.windows
    INTERFACE mini::Windows
)

target_sources(mini.windows
PUBLIC
    FILE_SET windows TYPE CXX_MODULES
    FILES
        windows_shared_ptr.cxx
        windows_handle.cxx
        windows_window.cxx
        windows.cxx

PRIVATE
    windows_include.h
    impl/windows_handle.cpp
    impl/windows_window.cpp
    impl/windows.cpp
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
    windows_include.h
)

build_source_tree(mini.windows)