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
    set(${out} ${prog})
    unset(prog)
    unset(${name}_prog)
endfunction()