add_compile_definitions(
    DEBUG=$<CONFIG:Debug>
    CLANG=$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>
    GNUC=$<CXX_COMPILER_ID:GNU>
    MSVC=$<CXX_COMPILER_ID:MSVC>
    APPLE_CLANG=$<CXX_COMPILER_ID:AppleClang>

    PLATFORM_WINDOWS=$<PLATFORM_ID:Windows>
    PLATFORM_MACOS=$<PLATFORM_ID:Darwin>
    PLATFORM_LINUX=$<PLATFORM_ID:Linux>
)

if (WIN32)
    add_compile_definitions(
        _ALLOW_KEYWORD_MACROS
        "CHAR_T=wchar_t"
        "no_unique_address=msvc::no_unique_address"
    )
else()
    add_compile_definitions(
        "CHAR_T=char"
    )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    add_compile_definitions(
        "force_inline=msvc::forceinline"
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    add_compile_definitions(
        "force_inline=clang::always_inline"
    )
else()
    message(FATAL_ERROR "unsupproted compiler")
endif()