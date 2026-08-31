cmake_minimum_required(VERSION 4.0 FATAL_ERROR)

project(MiniEngine 
    LANGUAGES CXX
    VERSION 0.1.0
)

option(BUILD_SHARED_LIBS "build modules in shared libraries" ON)
option(BUILD_MONOLITHIC "build all in single executable binary" OFF)
option(INCLUDE_TEST "include testing executables" OFF)
option(INCLUDE_BENCHMARK "include benchmarking executables" OFF)

set(ENGINE_PROJECT_NAME ${CMAKE_PROJECT_NAME})
set(ENGINE_PROJECT_VERSION ${CMAKE_PROJECT_VERSION})
set(ENGINE_PROJECT_AUTHOR "ross1573")

set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS FALSE)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

set(ENGINE_PROJECT_DIR "${CMAKE_SOURCE_DIR}")
set(ENGINE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/engine/source")
set(ENGINE_EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/engine/external")

set(BUILD_TYPES "Debug" "Develop" "Release")
set(BUILD_TARGET_ARCH ${CMAKE_SYSTEM_PROCESSOR})
set(MODULE_OUTPUT_SUFFIX "$<$<NOT:$<CONFIG:Release>>:.$<LOWER_CASE:$<CONFIG>>>")
set(BUNDLE_OUTPUT_NAME "${PROJECT_NAME}$<$<NOT:$<CONFIG:Release>>:$<CONFIG>>")

set(CMAKE_INSTALL_REMOVE_ENVIRONMENT_RPATH TRUE)
set(CMAKE_BUILD_REMOVE_ENVIRONMENT_RPATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
get_property(using_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
set(USING_MULTI_CONFIG ${using_multi_config})

list(APPEND CMAKE_MODULE_PATH "${ENGINE_PROJECT_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${ENGINE_PROJECT_DIR}/cmake/modules")

include(mini_build)
include(mini_util)

clone_build_type(Debug Develop)
apply_build_types()
validate_compiler()
set_output_directory()

add_subdirectory(${ENGINE_EXTERNAL_DIR})
add_subdirectory(${ENGINE_SOURCE_DIR})

if (INCLUDE_TEST)
    enable_testing()
    add_subdirectory_analyzed(test)
endif()

if (INCLUDE_BENCHMARK)
    add_subdirectory_analyzed(bench)
endif()