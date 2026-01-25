macro (module_global_definitions)
    set_property(GLOBAL APPEND PROPERTY MODULE_DEFINITIONS ${ARGV} "\n")
endmacro()

if (WIN32)
    if (BUILD_TARGET_ARCH MATCHES "[Aa][Rr][Mm]64")
        set(ARM64 1)
    elseif (BUILD_TARGET_ARCH MATCHES "[Aa][Mm][Dd]64|[Ii][Aa]64")
        set(X86 1)
        set(X86_64 1)
    elseif (BUILD_TARGET_ARCH MATCHES "[Xx]86")
        set(X86 1)
        set(X86_32 1)
    else()
        message(FATAL_ERROR "unsupproted arch: " ${BUILD_TARGET_ARCH})
    endif()
elseif (APPLE)
    if (BUILD_TARGET_ARCH MATCHES "[Aa][Rr][Mm]64")
        set(ARM64 1)
    elseif (BUILD_TARGET_ARCH MATCHES "[Xx]86_64")
        set(X86 1)
        set(X86_64 1)
    else()
        message(FATAL_ERROR "unsupproted arch: " ${BUILD_TARGET_ARCH})
    endif()
endif()

# define compiler specific compile definitions
# compiler generator expression aren't supported on file generate, 
# so they have to be handled manually
if (CMAKE_CXX_COMPILER_ID MATCHES AppleClang)
    set(APPLE_CLANG 1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set(CLANG 1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES GNUC)
    set(GNUC 1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    set(MSVC 1)
else()
    message(FATAL_ERROR "unsupproted compiler: " ${CMAKE_CXX_COMPILER_ID})
endif()

module_global_definitions(
    ENGINE_PROJECT_NAME="${ENGINE_PROJECT_NAME}"
    ENGINE_PROJECT_VERSION="${ENGINE_PROJECT_VERSION}"
)

module_global_definitions(
    DEBUG=$<IF:$<CONFIG:Debug>,true,false>
)

# handle compiler specific definition here
module_global_definitions(
    CLANG=$<IF:$<BOOL:${CLANG}>,true,false>
    GNUC=$<IF:$<BOOL:${GNUC}>,true,false>
    MSVC=$<IF:$<BOOL:${MSVC}>,true,false>
    APPLE_CLANG=$<IF:$<BOOL:${APPLE_CLANG}>,true,false>
)

module_global_definitions(
    PLATFORM_WINDOWS=$<IF:$<PLATFORM_ID:Windows>,true,false>
    PLATFORM_MACOS=$<IF:$<PLATFORM_ID:Darwin>,true,false>
    PLATFORM_LINUX=$<IF:$<PLATFORM_ID:Linux>,true,false>
)

module_global_definitions(
    ARCH_ARM64=$<IF:$<BOOL:${ARM64}>,true,false>
    ARCH_X86=$<IF:$<BOOL:${X86}>,true,false>
    ARCH_X86_64=$<IF:$<BOOL:${X86_64}>,true,false>
    ARCH_X86_32=$<IF:$<BOOL:${X86_32}>,true,false>
)

if (MSVC)
    module_global_definitions(
        force_inline=msvc::forceinline
        no_inline=msvc::noinline
        emptyable_address=msvc::no_unique_address
        "diagnose(cond, msg, level)"
        "diagnose_warning(cond, msg)"
        "diagnose_error(cond, msg)"
    )
elseif (CLANG)
    module_global_definitions(
        force_inline=clang::always_inline
        no_inline=clang::noinline
        emptyable_address=no_unique_address
        "diagnose(cond, msg, level)=clang::diagnose_if(cond, msg, level)"
        "diagnose_warning(cond, msg)=clang::diagnose_if(cond, msg, \"warn\")"
        "diagnose_error(cond, msg)=clang::diagnose_if(cond, msg, \"error\")"
    )
endif()