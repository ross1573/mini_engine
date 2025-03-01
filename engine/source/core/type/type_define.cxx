module;

#include <cstdint>
#include <cstddef>

export module mini.core:type_define;

template <typename T, T val>
struct IntegralConstant
{
    using ValueT = T;
    inline static constexpr ValueT value = val;
    inline constexpr operator ValueT() const noexcept { return val; }
    inline constexpr ValueT operator()() const noexcept { return val; }
};

export namespace mini
{

using byte = std::uint8_t;

using char8 = char8_t;
using char16 = char16_t;
using char32 = char32_t;

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

using TrueT = IntegralConstant<bool, true>;
using FalseT = IntegralConstant<bool, false>;
using NullptrT = std::nullptr_t;

} // namespace mini