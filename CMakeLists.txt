cmake_minimum_required(VERSION 4.0 FATAL_ERROR)

project(MiniEngine 
    LANGUAGES CXX
    VERSION 0.1.0
)

option(BUILD_SHARED_LIBS "build modules in shared libraries" ON)
option(INCLUDE_TEST "include testing executables" OFF)
option(INCLUDE_BENCHMARK "include benchmarking executables" OFF)

set(ENGINE_PROJECT_NAME ${CMAKE_PROJECT_NAME})
set(ENGINE_PROJECT_VERSION ${CMAKE_PROJECT_VERSION})
set(ENGINE_PROJECT_AUTHOR "ross1573")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS FALSE)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

set(BUILD_TARGET_ARCH ${CMAKE_SYSTEM_PROCESSOR})
set(BUILD_MODULE_PREFIX $<IF:$<CONFIG:Debug>,minid,mini>)

set(ENGINE_PROJECT_DIR "${CMAKE_SOURCE_DIR}")
set(ENGINE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/engine/source")
set(ENGINE_EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/engine/external")
set(ENGINE_ARCHIVE_DIR "${ENGINE_PROJECT_DIR}/lib")
set(ENGINE_LIBRARY_DIR "${ENGINE_PROJECT_DIR}/lib")
set(ENGINE_RUNTIME_DIR "${ENGINE_PROJECT_DIR}/bin")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "build")

list(APPEND CMAKE_MODULE_PATH "${ENGINE_PROJECT_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${ENGINE_PROJECT_DIR}/cmake/modules")

if (APPLE)
    enable_language(OBJCXX)
endif()

add_subdirectory(${ENGINE_EXTERNAL_DIR})
add_subdirectory(${ENGINE_SOURCE_DIR})

if (INCLUDE_TEST)
    enable_testing()
    add_subdirectory(test)
endif()

if (INCLUDE_BENCHMARK)
    add_subdirectory(bench)
endif()