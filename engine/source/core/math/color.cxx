export module mini.core:color;

import :type;
import :math_type;
import :math_operation;

namespace mini {

export struct Color {
public:
    union {
        struct {
            float32 r;
            float32 g;
            float32 b;
            float32 a;
        };

        float32 data[4];
    };

    constexpr Color() noexcept;
    constexpr Color(float32, float32, float32, float32) noexcept;
    explicit constexpr Color(Float32ConvertibleT auto const) noexcept;

    constexpr Color& operator+=(Color const&) noexcept;
    constexpr Color& operator-=(Color const&) noexcept;
    constexpr Color& operator*=(Float32ConvertibleT auto) noexcept;
    constexpr Color& operator/=(Float32ConvertibleT auto) noexcept;

    constexpr Color operator+() const noexcept;
    constexpr Color operator-() const noexcept;

    constexpr bool operator==(Color const&) const noexcept;

    static constexpr Color White() noexcept { return Color(1.f, 1.f, 1.f, 1.f); }
    static constexpr Color Black() noexcept { return Color(0.f, 0.f, 0.f, 1.f); }
    static constexpr Color Clear() noexcept { return Color(0.f, 0.f, 0.f, 0.f); }
    static constexpr Color Red() noexcept { return Color(1.f, 0.f, 0.f, 1.f); }
    static constexpr Color Green() noexcept { return Color(0.f, 1.f, 0.f, 1.f); }
    static constexpr Color Blue() noexcept { return Color(0.f, 0.f, 1.f, 1.f); }
};

export constexpr Color operator+(Color const&, Color const&) noexcept;
export constexpr Color operator-(Color const&, Color const&) noexcept;
export constexpr Color operator*(Color const&, Float32ConvertibleT auto const) noexcept;
export constexpr Color operator/(Color const&, Float32ConvertibleT auto const) noexcept;
export constexpr Color operator*(Float32ConvertibleT auto const, Color const&) noexcept;

inline constexpr Color::Color() noexcept
    : r(0.f)
    , g(0.f)
    , b(0.f)
    , a(0.f)
{
}

inline constexpr Color::Color(float32 inR, float32 inG, float32 inB, float32 inA) noexcept
    : r(inR)
    , g(inG)
    , b(inB)
    , a(inA)
{
}

inline constexpr Color::Color(Float32ConvertibleT auto const val) noexcept
    : r(static_cast<float32>(val))
    , g(static_cast<float32>(val))
    , b(static_cast<float32>(val))
    , a(static_cast<float32>(val))
{
}

inline constexpr Color& Color::operator+=(Color const& c) noexcept
{
    r += c.r;
    g += c.g;
    b += c.b;
    a += c.a;
    return *this;
}

inline constexpr Color& Color::operator-=(Color const& c) noexcept
{
    r -= c.r;
    g -= c.g;
    b -= c.b;
    a -= c.a;
    return *this;
}

inline constexpr Color& Color::operator*=(Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    r *= fVal;
    g *= fVal;
    b *= fVal;
    a *= fVal;
    return *this;
}

inline constexpr Color& Color::operator/=(Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    r /= fVal;
    g /= fVal;
    b /= fVal;
    a /= fVal;
    return *this;
}

inline constexpr Color Color::operator+() const noexcept
{
    return Color(r, g, b, a);
}

inline constexpr Color Color::operator-() const noexcept
{
    return Color(-r, -g, -b, -a);
}

inline constexpr bool Color::operator==(Color const& c) const noexcept
{
    return r == c.r && g == c.g && b == c.b && a == c.a;
}

inline constexpr Color operator+(Color const& c1, Color const& c2) noexcept
{
    Color result;
    result.r = c1.r + c2.r;
    result.g = c1.g + c2.g;
    result.b = c1.b + c2.b;
    result.a = c1.a + c2.a;
    return result;
}

inline constexpr Color operator-(Color const& c1, Color const& c2) noexcept
{
    Color result;
    result.r = c1.r - c2.r;
    result.g = c1.g - c2.g;
    result.b = c1.b - c2.b;
    result.a = c1.a - c2.a;
    return result;
}

inline constexpr Color operator*(Color const& c, Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Color result;
    result.r = c.r * fVal;
    result.g = c.g * fVal;
    result.b = c.b * fVal;
    result.a = c.a * fVal;
    return result;
}

inline constexpr Color operator/(Color const& c, Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Color result;
    result.r = c.r / fVal;
    result.g = c.g / fVal;
    result.b = c.b / fVal;
    result.a = c.a / fVal;
    return result;
}

inline constexpr Color operator*(Float32ConvertibleT auto val, Color const& c) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Color result;
    result.r = c.r * fVal;
    result.g = c.g * fVal;
    result.b = c.b * fVal;
    result.a = c.a * fVal;
    return result;
}

} // namespace mini