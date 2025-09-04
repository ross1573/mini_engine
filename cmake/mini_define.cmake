add_compile_definitions(
    DEBUG=$<CONFIG:Debug>
    LIB_PREFIX="${ENGINE_MODULE_PREFIX}"

    CLANG=$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>
    GNUC=$<CXX_COMPILER_ID:GNU>
    MSVC=$<CXX_COMPILER_ID:MSVC>
    APPLE_CLANG=$<CXX_COMPILER_ID:AppleClang>

    PLATFORM_WINDOWS=$<PLATFORM_ID:Windows>
    PLATFORM_MACOS=$<PLATFORM_ID:Darwin>
    PLATFORM_LINUX=$<PLATFORM_ID:Linux>
)

if (WIN32)
    # msvc shit needs to be applied ethier vs or clang
    add_compile_definitions(
        _ALLOW_KEYWORD_MACROS
        "no_unique_address=msvc::no_unique_address"
    )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    add_compile_definitions(
        "force_inline=msvc::forceinline"
        "no_inline=msvc::noinline"
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    add_compile_definitions(
        "force_inline=clang::always_inline"
        "no_inline=clang::noinline"
    )
else()
    message(FATAL_ERROR "unsupproted compiler: " ${CMAKE_CXX_COMPILER_ID})
endif()