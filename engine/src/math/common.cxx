module;

#include "mini_engine.h"
#include "core/define.h"
#include "core/type.h"

export module mini.math:common;

export namespace mini
{

template <FloatT T>
constexpr float Pow(T base, ConvertibleToT<T> auto exp)
{
    return std::pow(base, static_cast<float32>(exp));
}

template <IntT T>
constexpr OffsetT Pow(T base, T exp)
{
    OffsetT expInt = static_cast<SizeT>(exp);
    OffsetT result = 1;

    while (expInt)
    {
        if (expInt & 1)
        {
            result *= base;
        }

        expInt >>= 1;
        base *= base;
    }

    return result;
}

template <FloatT T>
/*constexpr*/ inline T Sqrt(T value)
{
    return std::sqrt(value);
}

template <typename T>
constexpr T const& Min(T const& val1, T const& val2)
{
    return val1 < val2 ? val1 : val2;
}

template <typename T>
constexpr T const& Max(T const& val1, T const& val2)
{
    return val1 > val2 ? val1 : val2;
}

} // export mini