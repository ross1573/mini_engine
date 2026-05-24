set(BUILD_TARGET_ARCH ${CMAKE_SYSTEM_PROCESSOR})
set(BUILD_PREFIX $<$<CONFIG:Debug>:d>)
set(BUILD_TYPES "Debug" "Develop" "Release")

get_property(using_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
set(USING_MULTI_CONFIG ${using_multi_config})

function (clone_build_type to from)
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

clone_build_type(Develop Debug)

if (CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES ${BUILD_TYPES} CACHE STRING "" FORCE)
else()
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${BUILD_TYPES})
    if (NOT ${CMAKE_BUILD_TYPE} IN_LIST BUILD_TYPES)
        message(FATAL_ERROR "unknown build type " ${CMAKE_BUILD_TYPE})
    endif()
endif()

if (MSVC)
    if (MSVC_VERSION LESS VC_VERSION_MIN)
        message(FATAL_ERROR "unsupported visual c++ version ${MSVC_VERSION}. "
                            "requires at least ${VC_VERSION_MIN}.")
    endif()
    if (MSVC_VERSION GREATER VC_VERSION_MAX)
        message(FATAL_ERROR "unsupported visual c++ version ${MSVC_VERSION}. "
                            "requires at most ${VC_VERSION_MAX}.")
    endif()

    message("-- Selecting MSVC version ${MSVC_VERSION}.")
endif()