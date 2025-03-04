add_module(mini.windows SHARED)

include(windows_header_define)

target_precompile_headers(mini.windows
PRIVATE
    <windows.h>
    <unknwn.h>
)

target_sources(mini.windows
PUBLIC
    FILE_SET windows TYPE CXX_MODULES
    FILES
        windows.cxx

PRIVATE
    win_handle.cpp
    win_window.cpp

PRIVATE
    FILE_SET windows_private TYPE CXX_MODULES
    FILES
        win_handle.cxx
        win_shared_ptr.cxx
        win_string.cxx
        win_window.cxx
)

target_link_libraries(mini.windows 
PUBLIC
    mini.core
    mini.platform

PRIVATE
    mini.graphics
    mini.engine
)

build_source_tree(mini.windows)

if (MSVC)
    set_source_files_properties(win_handle.cxx PROPERTIES COMPILE_FLAGS /wd4191)
endif()