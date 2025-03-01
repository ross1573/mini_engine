module;

#include <cmath>

export module mini.core:math;

import :type;

export namespace mini
{

template <typename T>
concept Int8ConvertibleT = ConvertibleToT<T, int8>;

template <typename T>
concept Int16ConvertibleT = ConvertibleToT<T, int16>;

template <typename T>
concept Int32ConvertibleT = ConvertibleToT<T, int32>;

template <typename T>
concept Int64ConvertibleT = ConvertibleToT<T, int64>;

template <typename T>
concept Uint8ConvertibleT = ConvertibleToT<T, uint8>;

template <typename T>
concept Uint16ConvertibleT = ConvertibleToT<T, uint16>;

template <typename T>
concept Uint32ConvertibleT = ConvertibleToT<T, uint32>;

template <typename T>
concept Uint64ConvertibleT = ConvertibleToT<T, uint64>;

template <typename T>
concept Float32ConvertibleT = ConvertibleToT<T, float32>;

template <typename T>
concept Float64ConvertibleT = ConvertibleToT<T, float64>;

template <FloatT T>
inline constexpr float Pow(T base, ConvertibleToT<T> auto exp)
{
    return std::pow(base, static_cast<float32>(exp));
}

template <IntT T>
inline constexpr OffsetT Pow(T base, T exp)
{
    if (std::is_constant_evaluated())
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
    else
    {
        return static_cast<OffsetT>(Pow(static_cast<float32>(base),
                                        static_cast<float32>(exp)));
    }
}

template <FloatT T>
/*constexpr*/ inline T Sqrt(T value)
{
    return std::sqrt(value);
}

template <typename T>
inline constexpr T const& Min(T const& val1, T const& val2)
{
    return val1 < val2 ? val1 : val2;
}

template <typename T>
inline constexpr T const& Max(T const& val1, T const& val2)
{
    return val1 > val2 ? val1 : val2;
}

} // export mini