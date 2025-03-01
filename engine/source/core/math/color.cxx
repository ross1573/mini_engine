export module mini.core:color;

import :type;
import :math;

export namespace mini
{

struct Color
{
    union
    {
        struct
        {
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

    static const Color white;
    static const Color black;
    static const Color clear;
    static const Color red;
    static const Color green;
    static const Color blue;
};

constexpr Color operator+(Color const&, Color const&) noexcept;
constexpr Color operator-(Color const&, Color const&) noexcept;
constexpr Color operator*(Color const&, Float32ConvertibleT auto const) noexcept;
constexpr Color operator/(Color const&, Float32ConvertibleT auto const) noexcept;
constexpr Color operator*(Float32ConvertibleT auto const, Color const&) noexcept;

inline Color const Color::white = Color(1.f, 1.f, 1.f, 1.f);
inline Color const Color::black = Color(0.f, 0.f, 0.f, 1.f);
inline Color const Color::clear = Color(0.f, 0.f, 0.f, 0.f);
inline Color const Color::red = Color(1.f, 0.f, 0.f, 1.f);
inline Color const Color::green = Color(0.f, 1.f, 0.f, 1.f);
inline Color const Color::blue = Color(0.f, 0.f, 1.f, 1.f);

inline constexpr Color::Color() noexcept
    : r(0.f), g(0.f), b(0.f), a(0.f)
{
}

inline constexpr Color::Color(float32 inR, float32 inG, float32 inB, float32 inA) noexcept
    : r(inR), g(inG), b(inB), a(inA)
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