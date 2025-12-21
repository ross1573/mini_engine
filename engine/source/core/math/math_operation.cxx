module;

#include "cmath.h"

export module mini.math:operation;

import mini.type;
import mini.bit;
import mini.memory;
import :type;
import :base;

namespace mini {

export template <IntegralT T>
[[nodiscard]] inline constexpr T Min(T x, T y)
{
    return x < y ? x : y;
}

export template <FloatingT T>
[[nodiscard]] inline constexpr T Min(T x, T y)
{
    if !consteval {
        if constexpr (sizeof(T) <= sizeof(float) && ConvertibleWithT<T, float>) {
            return static_cast<T>(BUILTIN_FMINF(static_cast<float>(x), static_cast<float>(y)));
        }
        else if constexpr (sizeof(T) <= sizeof(double) && ConvertibleWithT<T, double>) {
            return static_cast<T>(BUILTIN_FMIN(static_cast<double>(x), static_cast<double>(y)));
        }
        else if constexpr (sizeof(T) <= sizeof(long double) && ConvertibleWithT<T, long double>) {
            return static_cast<T>(BUILTIN_FMINL(static_cast<long double>(x),
                                                static_cast<long double>(y)));
        }
    }

    return x < y ? x : y;
}

export template <IntegralT T>
[[nodiscard]] inline constexpr T Max(T x, T y)
{
    return x > y ? x : y;
}

export template <FloatingT T>
[[nodiscard]] inline constexpr T Max(T x, T y)
{
    if !consteval {
        if constexpr (sizeof(T) <= sizeof(float) && ConvertibleWithT<T, float>) {
            return static_cast<T>(BUILTIN_FMAXF(static_cast<float>(x), static_cast<float>(y)));
        }
        else if constexpr (sizeof(T) <= sizeof(double) && ConvertibleWithT<T, double>) {
            return static_cast<T>(BUILTIN_FMAX(static_cast<double>(x), static_cast<double>(y)));
        }
        else if constexpr (sizeof(T) <= sizeof(long double) && ConvertibleWithT<T, long double>) {
            return static_cast<T>(BUILTIN_FMAXL(static_cast<long double>(x),
                                                static_cast<long double>(y)));
        }
    }

    return x > y ? x : y;
}

export template <IntegralT T>
[[nodiscard]] inline constexpr T Abs(T num)
{
    if constexpr (UnsignedT<T>) {
        return num;
    }

    if !consteval {
        if constexpr (sizeof(T) <= sizeof(int) && ConvertibleWithT<T, int>) {
            return static_cast<T>(BUILTIN_ABS(static_cast<int>(num)));
        }
        else if constexpr (sizeof(T) <= sizeof(long) && ConvertibleWithT<T, long>) {
            return static_cast<T>(BUILTIN_LABS(static_cast<long>(num)));
        }
        else if constexpr (sizeof(T) <= sizeof(long long) && ConvertibleWithT<T, long long>) {
            return static_cast<T>(BUILTIN_LLABS(static_cast<long long>(num)));
        }
    }

    return num < 0 ? static_cast<T>(0 - static_cast<UnsignedOfT<T>>(num)) : num;
}

export template <FloatingT T>
[[nodiscard]] inline constexpr T Abs(T num)
{
    if constexpr (UnsignedT<T>) {
        return num;
    }

    if !consteval {
        if constexpr (sizeof(T) <= sizeof(float) && ConvertibleWithT<T, float>) {
            return static_cast<T>(BUILTIN_FABSF(static_cast<float>(num)));
        }
        else if constexpr (sizeof(T) <= sizeof(double) && ConvertibleWithT<T, double>) {
            return static_cast<T>(BUILTIN_FABS(static_cast<double>(num)));
        }
        else if constexpr (sizeof(T) <= sizeof(long double) && ConvertibleWithT<T, long double>) {
            return static_cast<T>(BUILTIN_FABSL(static_cast<long double>(num)));
        }
    }

    return num < 0 ? -num : num;
}

export template <FloatingT T>
[[nodiscard]] inline /*constexpr*/ T Pow(T base, T exp)
{
    if constexpr (sizeof(T) <= sizeof(float) && ConvertibleWithT<T, float>) {
        return static_cast<T>(BUILTIN_POWF(static_cast<float>(base), static_cast<float>(exp)));
    }
    else if constexpr (sizeof(T) <= sizeof(double) && ConvertibleWithT<T, double>) {
        return static_cast<T>(BUILTIN_POW(static_cast<double>(base), static_cast<double>(exp)));
    }
    else if constexpr (sizeof(T) <= sizeof(long double) && ConvertibleWithT<T, long double>) {
        return static_cast<T>(BUILTIN_POWL(static_cast<long double>(base),
                                           static_cast<long double>(exp)));
    }
    else {
        NEVER_CALLED("not supported on such type", T);
    }
}

template <IntegralT T>
consteval auto PowIntResultTypeImpl()
{
    if constexpr (SignedT<T>) {
        return OffsetT(0);
    }
    else {
        return SizeT(0);
    }
}

export template <IntegralT T, IntegralT U>
[[nodiscard]] inline constexpr auto PowInt(T base, U exp)
    requires UnsignedT<U>
{
    using ResultT = decltype(PowIntResultTypeImpl<T>());
    ResultT result = 1;

    while (exp) {
        if (exp & 1) {
            result *= base;
        }

        exp >>= 1;
        base *= base;
    }

    return result;
}

export template <FloatingT T>
[[nodiscard]] inline /*constexpr*/ T Sqrt(T num)
{
    if constexpr (sizeof(T) <= sizeof(float) && ConvertibleWithT<T, float>) {
        return static_cast<T>(BUILTIN_SQRTF(static_cast<float>(num)));
    }
    else if constexpr (sizeof(T) <= sizeof(double) && ConvertibleWithT<T, double>) {
        return static_cast<T>(BUILTIN_SQRT(static_cast<double>(num)));
    }
    else if constexpr (sizeof(T) <= sizeof(long double) && ConvertibleWithT<T, long double>) {
        return static_cast<T>(BUILTIN_SQRTL(static_cast<long double>(num)));
    }
    else {
        NEVER_CALLED("not supported on such type", T);
    }
}

export template <IntegralT T>
[[nodiscard]] inline /*constexpr*/ float Sqrt(T num)
{
    return static_cast<float>(BUILTIN_SQRT(static_cast<double>(num)));
}

export template <IntegralT T, IntegralT U>
[[nodiscard]] inline constexpr CommonT<T, U> Gcd(T x, U y)
{
    using ResultT = CommonT<T, U>;
    using UnsignedResultT = UnsignedOfT<ResultT>;

    UnsignedResultT ux = static_cast<UnsignedResultT>(Abs(x));
    UnsignedResultT uy = static_cast<UnsignedResultT>(Abs(y));

    if (ux < uy) {
        UnsignedResultT tmp = uy;
        uy = ux;
        ux = tmp;
    }

    if (uy == 0) {
        return static_cast<ResultT>(ux);
    }

    ux %= uy;
    if (ux == 0) {
        return static_cast<ResultT>(uy);
    }

    UnsignedResultT uc = ux | uy;
    int32 shift = static_cast<int32>(bit::CountRightZero(uc));
    ux >>= bit::CountRightZero(ux);

    do {
        UnsignedResultT tmp = uy >> bit::CountRightZero(uy);
        if (ux > tmp) {
            uy = ux - tmp;
            ux = tmp;
        }
        else {
            uy = tmp - ux;
        }
    } while (uy != 0);

    return static_cast<ResultT>(ux << shift);
}

export template <IntegralT T, IntegralT U>
[[nodiscard]] inline constexpr CommonT<T, U> Lcm(T x, U y)
{
    using ResultT = CommonT<T, U>;

    if (x == 0 || y == 0) {
        return 0;
    }

    ResultT a = Abs(x) / Gcd(x, y);
    ResultT b = Abs(y);
    ResultT result;

    VERIFY(!BUILTIN_MUL_OVERFLOW(a, b, memory::AddressOf(result)), "integer overflow in lcm");
    return result;
}

} // namespace mini