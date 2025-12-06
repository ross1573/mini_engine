#ifndef BUILTIN_CMATH_H
#define BUILTIN_CMATH_H

#include <cmath>

#if HAS_BUILTIN(__builtin_fminf)
#  define BUILTIN_FMINF(x, y) __builtin_fminf(x, y)
#else
#  define BUILTIN_FMINF(x, y) fmin(x, y)
#endif

#if HAS_BUILTIN(__builtin_fmin)
#  define BUILTIN_FMIN(x, y) __builtin_fmin(x, y)
#else
#  define BUILTIN_FMIN(x, y) fmin(x, y)
#endif

#if HAS_BUILTIN(__builtin_fminl)
#  define BUILTIN_FMINL(x, y) __builtin_fminl(x, y)
#else
#  define BUILTIN_FMINL(x, y) fmin(x, y)
#endif

#if HAS_BUILTIN(__builtin_fmaxf)
#  define BUILTIN_FMAXF(x, y) __builtin_fmaxf(x, y)
#else
#  define BUILTIN_FMAXF(x, y) fmax(x, y)
#endif

#if HAS_BUILTIN(__builtin_fmax)
#  define BUILTIN_FMAX(x, y) __builtin_fmax(x, y)
#else
#  define BUILTIN_FMAX(x, y) fmax(x, y)
#endif

#if HAS_BUILTIN(__builtin_fmaxl)
#  define BUILTIN_FMAXL(x, y) __builtin_fmaxl(x, y)
#else
#  define BUILTIN_FMAXL(x, y) fmax(x, y)
#endif

#if HAS_BUILTIN(__builtin_abs)
#  define BUILTIN_ABS(x) __builtin_abs(x)
#else
#  define BUILTIN_ABS(x) abs(x)
#endif

#if HAS_BUILTIN(__builtin_labs)
#  define BUILTIN_LABS(x) __builtin_labs(x)
#else
#  define BUILTIN_LABS(x) labs(x)
#endif

#if HAS_BUILTIN(__builtin_llabs)
#  define BUILTIN_LLABS(x) __builtin_llabs(x)
#else
#  define BUILTIN_LLABS(x) llabs(x)
#endif

#if HAS_BUILTIN(__builtin_fabsf)
#  define BUILTIN_FABSF(x) __builtin_fabsf(x)
#else
#  define BUILTIN_FABSF(x) fabsf(x)
#endif

#if HAS_BUILTIN(__builtin_fabs)
#  define BUILTIN_FABS(x) __builtin_fabs(x)
#else
#  define BUILTIN_FABS(x) fabs(x)
#endif

#if HAS_BUILTIN(__builtin_fabsl)
#  define BUILTIN_FABSL(x) __builtin_fabsl(x)
#else
#  define BUILTIN_FABSL(x) fabsl(x)
#endif

#if HAS_BUILTIN(__builtin_powf)
#  define BUILTIN_POWF(x, y) __builtin_powf(x, y)
#else
#  define BUILTIN_POWF(x, y) powf(x, y)
#endif

#if HAS_BUILTIN(__builtin_pow)
#  define BUILTIN_POW(x, y) __builtin_pow(x, y)
#else
#  define BUILTIN_POW(x, y) pow(x, y)
#endif

#if HAS_BUILTIN(__builtin_powl)
#  define BUILTIN_POWL(x, y) __builtin_powl(x, y)
#else
#  define BUILTIN_POWL(x, y) powl(x, y)
#endif

#if HAS_BUILTIN(__builtin_sqrtf)
#  define BUILTIN_SQRTF(x) __builtin_sqrtf(x)
#else
#  define BUILTIN_SQRTF(x) sqrtf(x)
#endif

#if HAS_BUILTIN(__builtin_sqrt)
#  define BUILTIN_SQRT(x) __builtin_sqrt(x)
#else
#  define BUILTIN_SQRT(x) sqrt(x)
#endif

#if HAS_BUILTIN(__builtin_sqrtl)
#  define BUILTIN_SQRTL(x) __builtin_sqrtl(x)
#else
#  define BUILTIN_SQRTL(x) sqrtl(x)
#endif

#if HAS_BUILTIN(__builtin_add_overflow)
#  define BUILTIN_ADD_OVERFLOW(x, y, r) __builtin_add_overflow(x, y, r)
#else
#  define BUILTIN_ADD_OVERFLOW(x, y, r) AddOverflow(x, y, r)
#endif

#if HAS_BUILTIN(__builtin_sub_overflow)
#  define BUILTIN_SUB_OVERFLOW(x, y, r) __builtin_sub_overflow(x, y, r)
#else
#  define BUILTIN_SUB_OVERFLOW(x, y, r) SubOverflow(x, y, r)
#endif

#if HAS_BUILTIN(__builtin_mul_overflow)
#  define BUILTIN_MUL_OVERFLOW(x, y, r) __builtin_mul_overflow(x, y, r)
#else
#  define BUILTIN_MUL_OVERFLOW(x, y, r) MulOverflow(x, y, r)
#endif

#endif // BUILTIN_CMATH_H