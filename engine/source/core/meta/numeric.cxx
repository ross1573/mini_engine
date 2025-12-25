module;

#include <cfloat>
#include <climits>
#include <concepts>
#include <type_traits>

export module mini.core:numeric;

import :type;

namespace mini {

export template <typename T>
concept IntegralT = std::integral<T> && (SameAsT<T, bool> == false);

export template <typename T>
concept FloatingT = std::floating_point<T>;

export template <typename T>
concept ArithmeticT = IntegralT<T> || FloatingT<T>;

export template <typename T>
concept SignedT = ArithmeticT<T> && std::is_signed_v<T>;

export template <typename T>
concept UnsignedT = ArithmeticT<T> && std::is_unsigned_v<T>;

export template <typename T>
concept SignedIntegralT = IntegralT<T> && SignedT<T>;

export template <typename T>
concept UnsignedIntegralT = IntegralT<T> && UnsignedT<T>;

export template <typename T>
concept CharT = AnyOfT<RemoveConstVolatileT<T>, char, wchar_t, char8_t, char16_t, char32_t>;

export template <ArithmeticT T>
using SignedOfT = std::make_signed_t<T>;

export template <ArithmeticT T>
using UnsignedOfT = std::make_unsigned_t<T>;

export template <ArithmeticT T>
class NumericLimit;

template <IntegralT T>
class NumericLimit<T> {
public:
    typedef T Type;

private:
    static constexpr byte char_bit = CHAR_BIT;
    static constexpr byte signed_flag = SignedT<Type> ? 1 : 0;

public:
    static constexpr uint32 digits = static_cast<int>((sizeof(Type) * char_bit) - signed_flag);
    static constexpr uint32 digits10 = digits * 3 / 10;
    static constexpr uint32 radix = 2;

    static constexpr Type min = signed_flag ? Type(Type(1) << digits) : 0;
    static constexpr Type max = signed_flag ? Type(Type(~0) ^ min) : Type(~0);
};

template <>
class NumericLimit<float32> {
public:
    typedef float32 Type;

public:
    static constexpr uint32 digits = FLT_MANT_DIG;
    static constexpr uint32 digits10 = 2 + (digits * 30103l) / 100000l;
    static constexpr uint32 radix = FLT_RADIX;

    static constexpr Type min = -FLT_MAX;
    static constexpr Type max = FLT_MAX;
    static constexpr Type min_positive = FLT_MIN;
    static constexpr Type min_denorm = FLT_TRUE_MIN;
    static constexpr Type epsilon = FLT_EPSILON;
};

template <>
class NumericLimit<float64> {
public:
    typedef float64 Type;

public:
    static constexpr uint32 digits = DBL_MANT_DIG;
    static constexpr uint32 digits10 = 2 + (digits * 30103l) / 100000l;
    static constexpr uint32 radix = FLT_RADIX;

    static constexpr Type min = -DBL_MAX;
    static constexpr Type max = DBL_MAX;
    static constexpr Type min_positive = DBL_MIN;
    static constexpr Type min_denrom = DBL_TRUE_MIN;
    static constexpr Type epsilon = DBL_EPSILON;
};

template <typename T>
class NumericLimit<const T> : public NumericLimit<T> {};

template <typename T>
class NumericLimit<volatile T> : public NumericLimit<T> {};

template <typename T>
class NumericLimit<const volatile T> : public NumericLimit<T> {};

} // namespace mini