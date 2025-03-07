set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_MESSAGE_LOG_LEVEL WARNING) # silence messages from external source

macro (set_external name)
    set_target_properties(${name} PROPERTIES FOLDER external)
endmacro()

add_compile_options(
    /W0
    /utf-8
)

set(FMT_INSTALL OFF)
set(FMT_MODULE OFF)
add_subdirectory(fmt)
set_external(fmt)

add_subdirectory(convert_utf)
set_external(convert_utf)