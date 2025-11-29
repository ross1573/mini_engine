macro (add_module_definitions)
    list(APPEND COMPILE_DEFINITIONS ${ARGV} "\n")
endmacro()

if (WIN32)
    if (BUILD_TARGET_ARCH MATCHES "[Aa][Rr][Mm]64")
        set(ARM64 1)
    elseif (BUILD_TARGET_ARCH MATCHES "[Aa][Mm][Dd]64|[Ii][Aa]64")
        set(AMD64 1)
    elseif (BUILD_TARGET_ARCH MATCHES "[Xx]86")
        set(X64 1)
    else()
        message(FATAL_ERROR "unsupproted arch: " ${BUILD_TARGET_ARCH})
    endif()
elseif (APPLE)
    if (BUILD_TARGET_ARCH MATCHES "[Aa][Rr][Mm]64")
        set(ARM64 1)
    elseif (BUILD_TARGET_ARCH MATCHES "[Xx]86_64")
        set(AMD64 1)
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

add_module_definitions(
    ENGINE_PROJECT_NAME="${ENGINE_PROJECT_NAME}"
    ENGINE_PROJECT_VERSION="${ENGINE_PROJECT_VERSION}"
)

add_module_definitions(
    DEBUG=$<IF:$<CONFIG:Debug>,true,false>
)

# handle compiler specific definition here
add_module_definitions(
    CLANG=$<IF:$<BOOL:${CLANG}>,true,false>
    GNUC=$<IF:$<BOOL:${GNUC}>,true,false>
    MSVC=$<IF:$<BOOL:${MSVC}>,true,false>
    APPLE_CLANG=$<IF:$<BOOL:${APPLE_CLANG}>,true,false>
)

add_module_definitions(
    PLATFORM_WINDOWS=$<IF:$<PLATFORM_ID:Windows>,true,false>
    PLATFORM_MACOS=$<IF:$<PLATFORM_ID:Darwin>,true,false>
    PLATFORM_LINUX=$<IF:$<PLATFORM_ID:Linux>,true,false>
)

add_module_definitions(
    ARCH_ARM64=$<IF:$<BOOL:${ARM64}>,true,false>
    ARCH_X86_64=$<IF:$<BOOL:${AMD64}>,true,false>
    ARCH_X86=$<IF:$<BOOL:${X86}>,true,false>
)

if (WIN32)
    # msvc shit needs to be applied ethier vs or clang
    list(APPEND COMPILE_DEFINITIONS
        _ALLOW_KEYWORD_MACROS
        no_unique_address=msvc::no_unique_address
    )
endif()

if (MSVC)
    add_module_definitions(
        force_inline=msvc::forceinline
        no_inline=msvc::noinline
        "diagnose(cond, msg, level)"
        "diagnose_warning(cond, msg)"
        "diagnose_error(cond, msg)"
    )
elseif (CLANG)
    add_module_definitions(
        force_inline=clang::always_inline
        no_inline=clang::noinline
        "diagnose(cond, msg, level)=clang::diagnose_if(cond, msg, level)"
        "diagnose_warning(cond, msg)=clang::diagnose_if(cond, msg, \"warn\")"
        "diagnose_error(cond, msg)=clang::diagnose_if(cond, msg, \"error\")"
    )
endif()