#ifndef BUILTIN_BIT_H
#define BUILTIN_BIT_H

#include <bit>

#if MSVC
#  include <intrin.h>
#endif

#if HAS_BUILTIN(__builtin_bit_cast)
#  define BUILTIN_BIT_CAST(To, From) __builtin_bit_cast(To, From)
#else
#  define BUILTIN_BIT_CAST(To, From) std::bit_cast<To>(From)
#endif

#if HAS_BUILTIN(__builtin_rotateleft8)
#  define HAS_BUILTIN_ROTL_1   1
#  define BUILTIN_ROTL_1(x, s) __builtin_rotateleft8(x, static_cast<unsigned char>(s))
#elif MSVC
#  define HAS_BUILTIN_ROTL_1   1
#  define BUILTIN_ROTL_1(x, s) _rotl8(x, static_cast<unsigned char>(s))
#else
#  define HAS_BUILTIN_ROTL_1 0
#endif

#if HAS_BUILTIN(__builtin_rotateleft16)
#  define HAS_BUILTIN_ROTL_2   1
#  define BUILTIN_ROTL_2(x, s) __builtin_rotateleft16(x, static_cast<unsigned short>(s))
#elif MSVC
#  define HAS_BUILTIN_ROTL_2   1
#  define BUILTIN_ROTL_2(x, s) _rotl16(x, static_cast<unsigned char>(s))
#else
#  define HAS_BUILTIN_ROTL_2 0
#endif

#if HAS_BUILTIN(__builtin_rotateleft32)
#  define HAS_BUILTIN_ROTL_4   1
#  define BUILTIN_ROTL_4(x, s) __builtin_rotateleft32(x, static_cast<unsigned int>(s))
#elif MSVC
#  define HAS_BUILTIN_ROTL_4   1
#  define BUILTIN_ROTL_4(x, s) _rotl(x, s)
#else
#  define HAS_BUILTIN_ROTL_4 0
#endif

#if HAS_BUILTIN(__builtin_rotateleft64)
#  define HAS_BUILTIN_ROTL_8   1
#  define BUILTIN_ROTL_8(x, s) __builtin_rotateleft64(x, static_cast<unsigned long>(s))
#elif MSVC
#  define HAS_BUILTIN_ROTL_8   1
#  define BUILTIN_ROTL_8(x, s) _rotl64(x, s)
#else
#  define HAS_BUILTIN_ROTL_8 0
#endif

#if HAS_BUILTIN(__builtin_rotateright8)
#  define HAS_BUILTIN_ROTR_1   1
#  define BUILTIN_ROTR_1(x, s) __builtin_rotateright8(x, static_cast<unsigned char>(s))
#elif MSVC
#  define HAS_BUILTIN_ROTR_1   1
#  define BUILTIN_ROTR_1(x, s) _rotr8(x, static_cast<unsigned char>(s))
#else
#  define HAS_BUILTIN_ROTR_1 0
#endif

#if HAS_BUILTIN(__builtin_rotateright16)
#  define HAS_BUILTIN_ROTR_2   1
#  define BUILTIN_ROTR_2(x, s) __builtin_rotateright16(x, static_cast<unsigned short>(s))
#elif MSVC
#  define HAS_BUILTIN_ROTR_2   1
#  define BUILTIN_ROTR_2(x, s) _rotr16(x, static_cast<unsigned char>(s))
#else
#  define HAS_BUILTIN_ROTR_2 0
#endif

#if HAS_BUILTIN(__builtin_rotateright32)
#  define HAS_BUILTIN_ROTR_4   1
#  define BUILTIN_ROTR_4(x, s) __builtin_rotateright32(x, static_cast<unsigned int>(s))
#elif MSVC
#  define HAS_BUILTIN_ROTR_4   1
#  define BUILTIN_ROTR_4(x, s) _rotr(x, s)
#else
#  define HAS_BUILTIN_ROTR_4 0
#endif

#if HAS_BUILTIN(__builtin_rotateright64)
#  define HAS_BUILTIN_ROTR_8   1
#  define BUILTIN_ROTR_8(x, s) __builtin_rotateright64(x, static_cast<unsigned long>(s))
#elif MSVC
#  define HAS_BUILTIN_ROTR_8   1
#  define BUILTIN_ROTR_8(x, s) _rotr64(x, s)
#else
#  define HAS_BUILTIN_ROTR_8 0
#endif

#if HAS_BUILTIN(__builtin_clzg)
#  define HAS_BUILTIN_CLZG   1
#  define BUILTIN_CLZG(x, s) __builtin_clzg(x, s)
#else
#  define HAS_BUILTIN_CLZG 0
#endif

#if HAS_BUILTIN(__builtin_clz)
#  define BUILTIN_CLZ(x) __builtin_clz(x)
#elif MSVC
#  define BUILTIN_CLZ(x) _clz(x)
#else
#  define BUILTIN_CLZ(x) Clz(x)
#endif

#if HAS_BUILTIN(__builtin_clzl)
#  define BUILTIN_CLZL(x) __builtin_clzl(x)
#elif MSVC
#  define BUILTIN_CLZL(x) _clzl(x)
#else
#  define BUILTIN_CLZL(x) Clz(x)
#endif

#if HAS_BUILTIN(__builtin_clzll)
#  define BUILTIN_CLZLL(x) __builtin_clzll(x)
#elif MSVC
#  define BUILTIN_CLZLL(x) _clzll(x)
#else
#  define BUILTIN_CLZLL(x) Clz(x)
#endif

#if HAS_BUILTIN(__builtin_ctzg)
#  define HAS_BUILTIN_CTZG   1
#  define BUILTIN_CTZG(x, s) __builtin_ctzg(x, s)
#else
#  define HAS_BUILTIN_CTZG 0
#endif

#if HAS_BUILTIN(__builtin_ctz)
#  define BUILTIN_CTZ(x) __builtin_ctz(x)
#elif MSVC
#  define BUILTIN_CTZ(x) _ctz(x)
#else
#  define BUILTIN_CTZ(x) Ctz(x)
#endif

#if HAS_BUILTIN(__builtin_ctzl)
#  define BUILTIN_CTZL(x) __builtin_ctzl(x)
#elif MSVC
#  define BUILTIN_CTZL(x) _ctzl(x)
#else
#  define BUILTIN_CTZL(x) Ctz(x)
#endif

#if HAS_BUILTIN(__builtin_ctzll)
#  define BUILTIN_CTZLL(x) __builtin_ctzll(x)
#elif MSVC
#  define BUILTIN_CTZLL(x) _ctzll(x)
#else
#  define BUILTIN_CTZLL(x) Ctz(x)
#endif

#endif // BUILTIN_BIT_H