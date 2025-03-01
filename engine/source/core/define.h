#pragma once

#if defined(_WIN32) || defined(_WIN64)
#   define PLATFORM_WINDOWS 1
#else 
#   define PLATFORM_WINDOWS 0
#endif // _WIN32 || _WIN64

#if PLATFORM_WINDOWS
#   undef DEBUG
#   ifdef _DEBUG
#       define DEBUG 1
#   else
#       define DEBUG 0
#   endif
#   define DECL_DLL_SPEC(x) extern "C" { __declspec(dllexport) x; }
#   define CHAR_T wchar_t
#endif

#if defined(_MSC_VER)
#   define _ALLOW_KEYWORD_MACROS
#   define force_inline msvc::forceinline
#   define noinline msvc::noinline
#   define no_unique_address msvc::no_unique_address // crappy way of fucking the msvc compiler
#elif defined(__clang__)
#   define force_inline clang::always_inline
#   define noinline clang::noinline
#elif defined(__GNUC__)
#   define force_inline gnu::always_inline
#   define noinline gnu::noinline
#endif // attributess


// TODO: move to project settings
#ifdef _MSC_VER
#pragma warning(disable: 4061) // D3D12 never handles default case on switch
#pragma warning(disable: 4062)
#pragma warning(disable: 4165) // yea i'm fucking sure about it
#pragma warning(disable: 4201) // nameless struct/union
#pragma warning(disable: 4265) // windows api stuff
#pragma warning(disable: 4365) // ms doesn't care about signedness
#pragma warning(disable: 4514) // unreferenced inline function has been removed
#pragma warning(disable: 4625) // copy constructor implicitly deleted
#pragma warning(disable: 4626) // operator implicitly deleted
#pragma warning(disable: 4702) // unreachable code (from if constexpr)
#pragma warning(disable: 4710) // function not inlined
#pragma warning(disable: 4711) // selected for automatic inline
#pragma warning(disable: 4820) // implicit padding
#pragma warning(disable: 5045) // spectre stuff
#pragma warning(disable: 5204) // WRL stuff
#pragma warning(disable: 5266) // const prvalue warning
#pragma warning(disable: 6217) // fuck HRESULT
#pragma warning(disable: 6262) // i love stackoverflow
#endif // MSVC