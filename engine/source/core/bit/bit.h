#ifndef BUILTIN_BIT_H
#define BUILTIN_BIT_H

#if PLATFORM_WINDOWS
#  include <intrin.h>

#  define HAS_BUILTIN_ROTL_1 1
#  define HAS_BUILTIN_ROTL_2 1
#  define HAS_BUILTIN_ROTL_4 1
#  define HAS_BUILTIN_ROTL_8 1

#  define BUILTIN_ROTL_1(x, s) _rotr8(x, static_cast<unsigned char>(s))
#  define BUILTIN_ROTL_2(x, s) _rotl16(x, static_cast<unsigned char>(s))
#  define BUILTIN_ROTL_4(x, s) _rotl(x, s)
#  define BUILTIN_ROTL_8(x, s) _rotl64(x, s)

#  define HAS_BUILTIN_ROTR_1 1
#  define HAS_BUILTIN_ROTR_2 1
#  define HAS_BUILTIN_ROTR_4 1
#  define HAS_BUILTIN_ROTR_8 1

#  define BUILTIN_ROTR_1(x, s) _rotr8(x, static_cast<unsigned char>(s))
#  define BUILTIN_ROTR_2(x, s) _rotr16(x, static_cast<unsigned char>(s))
#  define BUILTIN_ROTR_4(x, s) _rotr(x, s)
#  define BUILTIN_ROTR_8(x, s) _rotr64(x, s)

#  define BUILTIN_CLZ(x)   _clz(x)
#  define BUILTIN_CLZL(x)  _clzl(x)
#  define BUILTIN_CLZLL(x) _clzll(x)

#  define BUILTIN_CTZ(x)   _ctz(x)
#  define BUILTIN_CTZL(x)  _ctzl(x)
#  define BUILTIN_CTZLL(x) _ctzll(x)
#else
#  define HAS_BUILTIN_ROTL_1 0
#  define HAS_BUILTIN_ROTL_2 0
#  define HAS_BUILTIN_ROTL_4 0
#  define HAS_BUILTIN_ROTL_8 0
#  define HAS_BUILTIN_ROTR_1 0
#  define HAS_BUILTIN_ROTR_2 0
#  define HAS_BUILTIN_ROTR_4 0
#  define HAS_BUILTIN_ROTR_8 0
#endif // PLATFORM_WINDOWS

#if HAS_BUILTIN(__builtin_clzg)
#  define HAS_BUILTIN_CLZG   1
#  define BUILTIN_CLZG(x, s) __builtin_clzg(x, s)
#else
#  define HAS_BUILTIN_CLZG 0
#endif

#ifndef BUILTIN_CLZ
#  if HAS_BUILTIN(__builtin_clz)
#    define BUILTIN_CLZ(x) __builtin_clz(x)
#  else
#    define BUILTIN_CLZ(x) Clz(x)
#  endif
#endif

#ifndef BUILTIN_CLZL
#  if HAS_BUILTIN(__builtin_clzl)
#    define BUILTIN_CLZL(x) __builtin_clzl(x)
#  else
#    define BUILTIN_CLZL(x) Clz(x)
#  endif
#endif

#ifndef BUILTIN_CLZLL
#  if HAS_BUILTIN(__builtin_clzll)
#    define BUILTIN_CLZLL(x) __builtin_clzll(x)
#  else
#    define BUILTIN_CLZLL(x) Clz(x)
#  endif
#endif

#if HAS_BUILTIN(__builtin_ctzg)
#  define HAS_BUILTIN_CTZG   1
#  define BUILTIN_CTZG(x, s) __builtin_ctzg(x, s)
#else
#  define HAS_BUILTIN_CTZG 0
#endif

#ifndef BUILTIN_CTZ
#  if HAS_BUILTIN(__builtin_ctz)
#    define BUILTIN_CTZ(x) __builtin_ctz(x)
#  else
#    define BUILTIN_CTZ(x) Ctz(x)
#  endif
#endif

#ifndef BUILTIN_CTZL
#  if HAS_BUILTIN(__builtin_ctzl)
#    define BUILTIN_CTZL(x) __builtin_ctzl(x)
#  else
#    define BUILTIN_CTZL(x) Ctz(x)
#  endif
#endif

#ifndef BUILTIN_CTZLL
#  if HAS_BUILTIN(__builtin_ctzll)
#    define BUILTIN_CTZLL(x) __builtin_ctzll(x)
#  else
#    define BUILTIN_CTZLL(x) Ctz(x)
#  endif
#endif

#endif // BUILTIN_BIT_H