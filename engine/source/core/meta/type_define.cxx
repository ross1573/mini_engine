module;

#include <cstddef>
#include <cstdint>

export module mini.core:type_define;

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

export using size_t = std::size_t;
export using offset_t = std::ptrdiff_t;
export using nullptr_t = std::nullptr_t;

} // namespace mini