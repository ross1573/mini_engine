add_library(convert_utf STATIC)

generate_export_header(convert_utf
    EXPORT_MACRO_NAME CVT_UTF_API
    EXPORT_FILE_NAME convert_utf_api.h
)

target_sources(convert_utf
PUBLIC
    FILE_SET cvt_utf TYPE CXX_MODULES
    FILES
        convert_utf.cxx

PRIVATE
    convert_utf.cpp
)

target_include_directories(convert_utf 
PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}

PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)