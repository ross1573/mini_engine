set(CMAKE_MESSAGE_LOG_LEVEL WARNING) # silence messages from external source
set(CMAKE_CXX_SCAN_FOR_MODULES FALSE) # most external libraries aren't compatible with c++ modules yet
set(BUILD_SHARED_LIBS FALSE) # build external as static library

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ENGINE_PROJECT_DIR}/lib/external")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ENGINE_PROJECT_DIR}/lib/external")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ENGINE_PROJECT_DIR}/bin/external")

macro (set_external name)
    set_target_properties(${name} PROPERTIES FOLDER external)
endmacro()

if (CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    add_compile_options(/W0 /utf-8)
elseif (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    add_compile_options(-Wno-everything)
endif()

set(FMT_INSTALL OFF)
set(FMT_MODULE ON)
add_subdirectory(fmt)
set_external(fmt)

add_subdirectory(convert_utf)
set_external(convert_utf)

if (APPLE)
    add_subdirectory(metal-cpp)
    set_external(metal.cpp)
endif()