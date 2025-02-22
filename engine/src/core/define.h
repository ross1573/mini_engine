#pragma once

#if defined(_MSC_VER)
#   ifndef MSVC
#   define MSVC
#   endif
#elif defined(__clang__)
#   ifndef CLANG
#   define CLANG
#   endif
#elif defined(__GNU__)
#   ifndef GNU
#   define GNU
#   endif
#endif // compiler

#if defined(_WIN32) || defined(_WIN64)
#ifndef PLATFORM_WINDOWS
#define PLATFORM_WINDOWS
#endif // _WIN32 || _WIN64

#ifndef _WIN64
#   ifndef WIN32
#   define WIN32
#   endif
#else // _WIN64
#   ifndef WIN64
#   define WIN64
#   endif
#endif // _WIN64

#ifdef _DEBUG
#   ifndef DEBUG
#   define DEBUG
#   endif
#   define _CRTDBG_MAP_ALLOC
#endif // _DEBUG

#ifdef NOASSERT
#   ifndef NDEBUG
#   define NDEBUG
#   endif
#endif // NOASSERT

#define NOMINMAX
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP
#define NOMCX
#define NOSERVICE
#define NOHELP
#define WIN32_LEAN_AND_MEAN
#define _ALLOW_KEYWORD_MACROS
#define _CRT_SECURE_NO_WARNINGS

#define DECL_DLL_SPEC(x) \
    extern "C" { __declspec(dllexport) x; }

#endif // _WIN32 || _WIN64

#ifdef MSVC
#pragma warning(disable: 4061) // D3D12 never handles default case on switch
#pragma warning(disable: 4062)
#pragma warning(disable: 4165) // yea i'm fucking sure about it
#pragma warning(disable: 4201) // nameless struct/union
#pragma warning(disable: 4265) // windows api stuff
#pragma warning(disable: 4365) // ms doesn't care about signedness
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

#if defined(MSVC)
#   define force_inline msvc::forceinline
#   define noinline msvc::noinline
#   define no_unique_address msvc::no_unique_address // crappy way of fucking the msvc compiler
#elif defined(CLANG)
#   define force_inline clang::always_inline
#   define noinline clang::noinline
#elif defined(GNU)
#   define force_inline gnu::always_inline
#   define noinline gnu::noinline
#endif // attributess

#ifndef MSVC 
#   ifndef PLATFORM_CHAR
#   define PLATFORM_CHAR char
#   endif
#else 
#   ifndef PLATFORM_CHAR
#   define PLATFORM_CHAR wchar_t
#   endif
#endif

namespace mini
{

using byte = std::uint8_t;

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

using uint = std::uint32_t;
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using float32 = float;
using float64 = double;

using SizeT = uint32;
using OffsetT = std::ptrdiff_t;

using TrueT = std::true_type;
using FalseT = std::false_type;
using NullptrT = std::nullptr_t;

} // namespace mini