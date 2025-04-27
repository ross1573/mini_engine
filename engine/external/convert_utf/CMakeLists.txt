add_library(convert_utf STATIC)

target_sources(convert_utf
PUBLIC
    FILE_SET cvt_utf TYPE CXX_MODULES
    FILES
        convert_utf.cxx
)

set_target_properties(convert_utf PROPERTIES CXX_SCAN_FOR_MODULES TRUE)