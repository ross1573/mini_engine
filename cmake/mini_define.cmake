include_guard()

macro (module_global_definitions)
     list(APPEND MODULE_GLOBAL_DEFINITIONS ${ARGV} "\n")
endmacro()

if (NOASSERT)
    set(assert 0)
elseif (ASSERT_LEVEL STREQUAL "Debug")
    set(assert "$<CONFIG:Debug>")
elseif (ASSERT_LEVEL STREQUAL "Develop")
    set(assert "$<CONFIG:Debug,Develop>")
elseif (ASSERT_LEVEL STREQUAL "Release")
    set(assert "$<CONFIG:Debug,Develop,Release>")
else()
    set(assert 1)
endif()

if (NODEBUGLOG)
    set(debugLog 0)
elseif (DEBUG_LOG_LEVEL STREQUAL "Debug")
    set(debugLog "$<CONFIG:Debug>")
elseif (DEBUG_LOG_LEVEL STREQUAL "Develop")
    set(debugLog "$<CONFIG:Debug,Develop>")
elseif (DEBUG_LOG_LEVEL STREQUAL "Release")
    set(debugLog "$<CONFIG:Debug,Develop,Release>")
else()
    set(debugLog 1)
endif()

module_global_definitions(
    ENGINE_PROJECT_NAME="${ENGINE_PROJECT_NAME}"
    ENGINE_PROJECT_AUTHOR="${ENGINE_PROJECT_AUTHOR}"
    ENGINE_PROJECT_VERSION="${ENGINE_PROJECT_VERSION}"
    ENGINE_BUILD_ARCH="${ENGINE_TARGET_ARCH}"
    ENGINE_BUILD_CONFIG="$<STRING:TOLOWER,$<CONFIG>>"
    ENGINE_BUILD_COMPILER="${ENGINE_COMPILER}:${CMAKE_CXX_COMPILER_VERSION}"
)

module_global_definitions(
    DEBUG=$<IF:$<CONFIG:Debug>,true,false>
    DEVELOP=$<IF:$<CONFIG:Develop>,true,false>
    RELEASE=$<IF:$<CONFIG:Release>,true,false>
    NOASSERT=$<IF:${assert},false,true>
    NODEBUGLOG=$<IF:${debugLog},false,true>
)

# handle compiler specific definition here
module_global_definitions(
    CLANG=$<IF:${CLANG},true,false>
    MSVC=$<IF:${MSVC},true,false>
)

module_global_definitions(
    PLATFORM_WINDOWS=$<IF:${WINDOWS},true,false>
    PLATFORM_MACOS=$<IF:${MACOS},true,false>
)

module_global_definitions(
    ARCH_ARM64=$<IF:${ARM64},true,false>
    ARCH_X86_64=$<IF:${X86_64},true,false>
)

if (MSVC)
    module_global_definitions(
        force_inline=msvc::forceinline
        no_inline=msvc::noinline
        emptyable_address=msvc::no_unique_address
        "deleted_function(msg)=delete"
        "static_diagnose(cond, msg, level)"
        "static_diagnose_warning(cond, msg)"
        "static_diagnose_error(cond, msg)"
    )
elseif (CLANG)
    module_global_definitions(
        force_inline=clang::always_inline
        no_inline=clang::noinline
        emptyable_address=no_unique_address
        "deleted_function(msg)=delete(msg)"
        "static_diagnose(cond, msg, level)=clang::diagnose_if(cond, msg, level)"
        "static_diagnose_warning(cond, msg)=clang::diagnose_if(cond, msg, \"warn\")"
        "static_diagnose_error(cond, msg)=clang::diagnose_if(cond, msg, \"error\")"
    )
endif()