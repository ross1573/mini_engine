add_module(mini.windows SHARED)

include(windows_header_define)

target_precompile_headers(mini.windows
PRIVATE
    <windows.h>
    <unknwn.h>
)

module_sources(mini.windows
PUBLIC
    FILE_SET windows TYPE CXX_MODULES
    FILES
        windows.cxx
        win_handle.cxx
        win_shared_ptr.cxx
        win_window.cxx

PRIVATE
    win_handle.cpp
    win_window.cpp        
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