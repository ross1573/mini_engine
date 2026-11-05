macro (include_static name)
    set(prev_build_type ${BUILD_SHARED_LIBS})
    set(BUILD_SHARED_LIBS FALSE)
    include(${name})
    set(BUILD_SHARED_LIBS ${prev_build_type})
endmacro()

macro (include_shared name)
    set(prev_build_type ${BUILD_SHARED_LIBS})
    set(BUILD_SHARED_LIBS TRUE)
    include(${name})
    set(BUILD_SHARED_LIBS ${prev_build_type})
endmacro()

macro (timer_start name)
    string(REPLACE " " "_" ts_name ${name})
    string(TIMESTAMP ${ts_name}_start_s "%s")
    string(TIMESTAMP ${ts_name}_start_m "%f")
    math(EXPR ${ts_name}_start "${${ts_name}_start_s} * 1000000 + ${${ts_name}_start_m}")
endmacro()

macro (timer_end name)
    string(REPLACE " " "_" ts_name ${name})
    string(TIMESTAMP ${ts_name}_end_s "%s")
    string(TIMESTAMP ${ts_name}_end_m "%f")

    math(EXPR ${ts_name}_end "${${ts_name}_end_s} * 1000000 + ${${ts_name}_end_m}")
    math(EXPR ${ts_name}_elapsed "${${ts_name}_end} - ${${ts_name}_start}")
    math(EXPR ${ts_name}_sec "${${ts_name}_elapsed} / 1000000")
    math(EXPR ${ts_name}_mic "${${ts_name}_elapsed} / 100000")
endmacro()

macro (timer_print task name)
    string(REPLACE " " "_" ts_name ${name})
    message(STATUS "${task} done (${${ts_name}_sec}.${${ts_name}_mic}s)")
endmacro()

macro (add_subdirectory_analyzed name)
    timer_start(${name})
    add_subdirectory(${ARGV})
    timer_end(${name})
    timer_print("Configuring ${name}" ${name})
endmacro()

macro (snake_to_camel_case name)
    string(REPLACE "_" ";" list ${name})
    foreach (split ${list})
        string(SUBSTRING ${split} 0 1 first)
        string(SUBSTRING ${split} 1 -1 remain)
        string(TOUPPER ${first} first_upper)
        list(APPEND result ${first_upper} ${remain})
    endforeach()
    string(CONCAT camel_case ${result})
endmacro()

function (find_program_from_path name path out)
    set(prog ${name}_prog)
    get_property(prog GLOBAL PROPERTY "${name}_PROGRAM")
    if (prog STREQUAL "UNDEFINED")
        set(${out} "" PARENT_SCOPE)
        return()
    elseif (prog)
        set(${out} ${prog} PARENT_SCOPE)
        return()
    endif()

    find_program(prog ${path} ${name}
        PATHS ${path}
        OPTIONAL
        NO_CACHE
        NO_DEFAULT_PATH
        NO_PACKAGE_ROOT_PATH
        NO_CMAKE_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH
        NO_CMAKE_INSTALL_PREFIX)

    if (NOT prog)
        message(WARNING "failed to find dsymutil from given path (${path})")
        return()
    endif()

    set_property(GLOBAL PROPERTY "${name}_PROGRAM" ${prog})
    set(${out} ${prog} PARENT_SCOPE)
endfunction()

macro (_get_output_directory_impl archive_out library_out runtime_out)
    string(TOLOWER ${ENGINE_TARGET_ARCH} target_arch)
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
    string(TOLOWER ${ENGINE_TARGET_ARCH} target_arch)
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

    if (WINDOWS)
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