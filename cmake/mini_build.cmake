function (clone_build_type from to)
    if (NOT ${from} IN_LIST BUILD_TYPES)
        message(FATAL_ERROR "failed to clone build type ${from}")
    endif()

    string(TOUPPER ${from} from)
    string(TOUPPER ${to} to)

    get_directory_property(vars VARIABLES)
    foreach(var IN LISTS vars)
        if (var MATCHES "_${from}")
            string(REPLACE "_${from}" "_${to}" var_new "${var}")
            set(${var_new} "${${var}}" PARENT_SCOPE)
        endif()
    endforeach()
endfunction()

function (apply_build_types)
    if (CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_CONFIGURATION_TYPES ${BUILD_TYPES} CACHE STRING "" FORCE)
    else()
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${BUILD_TYPES})
        if (NOT ${CMAKE_BUILD_TYPE} IN_LIST BUILD_TYPES)
            message(FATAL_ERROR "unknown build type " ${CMAKE_BUILD_TYPE})
        endif()
    endif()
endfunction()

function (validate_compiler)
    if (CMAKE_CXX_COMPILER_ID MATCHES MSVC)
        if (MSVC_VERSION LESS VC_VERSION_MIN)
            message(FATAL_ERROR "unsupported visual c++ version ${MSVC_VERSION}. "
                                "requires at least ${VC_VERSION_MIN}.")
        endif()
        if (MSVC_VERSION GREATER VC_VERSION_MAX)
            message(FATAL_ERROR "unsupported visual c++ version ${MSVC_VERSION}. "
                                "requires at most ${VC_VERSION_MAX}.")
        endif()

        message("-- Selecting MSVC version ${MSVC_VERSION}.")
    elseif (CMAKE_CXX_COMPILER_ID MATCHES Clang)
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS CLANG_VERSION_MIN)
            message(FATAL_ERROR "unsupported clang version ${CMAKE_CXX_COMPILER_VERSION}. "
                                "requires at least ${CLANG_VERSION_MIN}.")
        elseif (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER CLANG_VERSION_MAX)
            message(FATAL_ERROR "unsupported clang version ${CMAKE_CXX_COMPILER_VERSION}. "
                                "requires at most ${CLANG_VERSION_MAX}.")
        endif()
    endif()
endfunction()

macro (_get_output_directory_impl archive_out library_out runtime_out)
    string(TOLOWER ${BUILD_TARGET_ARCH} target_arch)
    if (USING_MULTI_CONFIG)
        set(config_lower "$<STRING:TOLOWER,$<CONFIG>>")
    else()
        string(TOLOWER ${CMAKE_BUILD_TYPE} config_lower)
    endif()

    if (BUILD_MONOLITHIC)
        set(mono_build "_mono")
    else()
        set(mono_build "")
    endif()

    set(${archive_out} "${ENGINE_ARCHIVE_DIR}/${target_arch}_${config_lower}${mono_build}")
    set(${library_out} "${ENGINE_LIBRARY_DIR}/${target_arch}_${config_lower}${mono_build}")
    set(${runtime_out} "${ENGINE_RUNTIME_DIR}/${target_arch}_${config_lower}${mono_build}")
endmacro()

macro (_get_config_output_directory_impl config archive_out library_out runtime_out)
    string(TOLOWER ${BUILD_TARGET_ARCH} target_arch)
    string(TOLOWER ${config} config_lower)
    
    if (BUILD_MONOLITHIC)
        set(mono_build "_mono")
    else()
        set(mono_build "")
    endif()

    set(${archive_out} "${ENGINE_ARCHIVE_DIR}/${target_arch}_${config_lower}${mono_build}")
    set(${library_out} "${ENGINE_LIBRARY_DIR}/${target_arch}_${config_lower}${mono_build}")
    set(${runtime_out} "${ENGINE_RUNTIME_DIR}/${target_arch}_${config_lower}${mono_build}")
endmacro()

macro (_get_runtime_target_directory runtime_target_out)
    if (NOT DEFINED dir)
        message(FATAL_ERROR "dir not defined")
    endif()

    if (WIN32)
        set(${runtime_target_out} "")
    else()
        set(${runtime_target_out} ${dir})
    endif()
endmacro()

macro (_set_output_directory_impl)
    if (NOT DEFINED dir)
        message(FATAL_ERROR "dir not defined")
    endif()

    _get_runtime_target_directory(runtime_target_dir ${dir})

    if (USING_MULTI_CONFIG)
        foreach (type ${BUILD_TYPES})
            string(TOUPPER ${type} build_type_upper)
            _get_config_output_directory_impl(${type} archive_dir library_dir runtime_dir)

            set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${build_type_upper} "${archive_dir}/${dir}" PARENT_SCOPE)
            set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${build_type_upper} "${library_dir}/${dir}" PARENT_SCOPE)
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${build_type_upper} "${runtime_dir}/${runtime_target_dir}" PARENT_SCOPE)
        endforeach()
    else()
        _get_config_output_directory_impl(${CMAKE_BUILD_TYPE} archive_dir library_dir runtime_dir)

        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${archive_dir}/${dir}" PARENT_SCOPE)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${library_dir}/${dir}" PARENT_SCOPE)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${runtime_dir}/${runtime_target_dir}" PARENT_SCOPE)
    endif()
endmacro()

macro (_set_target_output_directory_impl target)
    if (NOT DEFINED dir)
        message(FATAL_ERROR "dir not defined")
    endif()

    _get_runtime_target_directory(runtime_target_dir ${dir})

    if (USING_MULTI_CONFIG)
        foreach (type ${BUILD_TYPES})
            string(TOUPPER ${type} build_type_upper)
            _get_config_output_directory_impl(${type} archive_dir library_dir runtime_dir)

            set_target_properties(${target} PROPERTIES
                ARCHIVE_OUTPUT_DIRECTORY_${build_type_upper} "${archive_dir}/${dir}"
                LIBRARY_OUTPUT_DIRECTORY_${build_type_upper} "${library_dir}/${dir}"
                RUNTIME_OUTPUT_DIRECTORY_${build_type_upper} "${runtime_dir}/${runtime_target_dir}")
        endforeach()
    else()
        _get_config_output_directory_impl(${CMAKE_BUILD_TYPE} archive_dir library_dir runtime_dir)

        set_target_properties(${target} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${archive_dir}/${dir}"
            LIBRARY_OUTPUT_DIRECTORY "${library_dir}/${dir}"
            RUNTIME_OUTPUT_DIRECTORY "${runtime_dir}/${runtime_target_dir}")
    endif()
endmacro()

function (get_output_directory out type)
    get_property(dir_stack GLOBAL PROPERTY OUTPUT_DIRECTORY_STACK)
    if (${ARGC} EQUAL 3)
        list(GET ARGV 2 dir)
    elseif (dir_stack)
        list(GET dir_stack -1 dir)
    else()
        set(dir "")
    endif()

    _get_output_directory_impl(archive_dir library_dir runtime_dir)
    _get_runtime_target_directory(runtime_target_dir ${dir})

    if (${type} STREQUAL "RUNTIME")
        set(${out} "${runtime_dir}/${runtime_target_dir}" PARENT_SCOPE)
    elseif (${type} STREQUAL "LIBRARY")
        set(${out} "${library_dir}/${dir}" PARENT_SCOPE)
    elseif (${type} STREQUAL "ARCHIVE")
        set(${out} "${archive_dir}/${dir}" PARENT_SCOPE)
    else()
        message(FATAL_ERROR "unknown type of output directory ${type}")
    endif()
endfunction()

function (set_output_directory)
    if (${ARGC} EQUAL 0)
        set(dir "")
        set_property(GLOBAL APPEND PROPERTY OUTPUT_DIRECTORY_STACK "UNDEFINED")
    elseif (${ARGC} EQUAL 1)
        list(GET ARGV 0 dir)
        set_property(GLOBAL APPEND PROPERTY OUTPUT_DIRECTORY_STACK ${dir})
    else()
        message(FATAL_ERROR "invalid arguments. expected 1, instead received ${ARGC}(${ARGV})")
    endif()

    _set_output_directory_impl()

    cmake_language(EVAL CODE "cmake_language(DEFER CALL restore_output_directory)")
endfunction()

function (set_target_output_directory target)
    if (${ARGC} EQUAL 1)
        set(dir "")
    elseif (${ARGC} EQUAL 2)
        list(GET ARGV 1 dir)
    else()
        message(FATAL_ERROR "invalid arguments. expected 2, instead received ${ARGC}(${ARGV})")
    endif()

    _set_target_output_directory_impl(${target})
endfunction()

function (restore_output_directory)
    get_property(dir_stack GLOBAL PROPERTY OUTPUT_DIRECTORY_STACK)
    list(POP_BACK dir_stack)
    if (dir_stack)
        list(GET dir_stack -1 dir)
    else()
        set(dir "UNDEFINED")
    endif()

    if (dir STREQUAL "UNDEFINED")
        set(dir "")
    endif()

    _set_output_directory_impl()
endfunction()

function (set_startup_project target)
    if (CMAKE_GENERATOR MATCHES "Visual Studio")
        set_property(DIRECTORY ${CMAKE_BINARY_DIR} PROPERTY VS_STARTUP_PROJECT ${target})
    endif()
endfunction()