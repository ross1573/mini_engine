module;

#include <cstddef>
#include <cstdint>

export module mini.core:type_define;

template <typename T, T val>
struct IntegralConstant {
    using ValueT = T;
    inline static constexpr ValueT value = val;
    inline constexpr operator ValueT() const noexcept { return val; }
    inline constexpr ValueT operator()() const noexcept { return val; }
};

namespace mini {

export using byte = std::uint8_t;

export using wchar = wchar_t;
export using char8 = char8_t;
export using char16 = char16_t;
export using char32 = char32_t;

export using int8 = std::int8_t;
export using int16 = std::int16_t;
export using int32 = std::int32_t;
export using int64 = std::int64_t;

export using uint8 = std::uint8_t;
export using uint16 = std::uint16_t;
export using uint32 = std::uint32_t;
export using uint64 = std::uint64_t;

export using float32 = float;
export using float64 = double;

export using SizeT = std::size_t;
export using OffsetT = std::ptrdiff_t;

export using TrueT = IntegralConstant<bool, true>;
export using FalseT = IntegralConstant<bool, false>;

export using IntPtrT = std::intptr_t;
export using NullptrT = std::nullptr_t;

} // namespace mini