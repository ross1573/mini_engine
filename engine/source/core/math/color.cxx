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
    constexpr Color(float32 r, float32 g, float32 b, float32 a) noexcept;
    explicit constexpr Color(Float32ConvertibleT auto val) noexcept;

    constexpr Color& operator+=(Color const& other) noexcept;
    constexpr Color& operator-=(Color const& other) noexcept;
    constexpr Color& operator*=(Float32ConvertibleT auto val) noexcept;
    constexpr Color& operator/=(Float32ConvertibleT auto val) noexcept;

    constexpr Color operator+() const noexcept;
    constexpr Color operator-() const noexcept;

    constexpr bool operator==(Color const& other) const noexcept;

    static constexpr Color White() noexcept { return { 1.0f, 1.f, 1.f, 1.f }; }
    static constexpr Color Black() noexcept { return { 0.f, 0.f, 0.f, 1.f }; }
    static constexpr Color Clear() noexcept { return { 0.f, 0.f, 0.f, 0.f }; }
    static constexpr Color Red() noexcept { return { 1.f, 0.f, 0.f, 1.f }; }
    static constexpr Color Green() noexcept { return { 0.f, 1.f, 0.f, 1.f }; }
    static constexpr Color Blue() noexcept { return { 0.f, 0.f, 1.f, 1.f }; }
};

export constexpr Color operator+(Color const& lhs, Color const& rhs) noexcept;
export constexpr Color operator-(Color const& lhs, Color const& rhs) noexcept;
export constexpr Color operator*(Color const& color, Float32ConvertibleT auto val) noexcept;
export constexpr Color operator/(Color const& color, Float32ConvertibleT auto val) noexcept;
export constexpr Color operator*(Float32ConvertibleT auto val, Color const& color) noexcept;

constexpr Color::Color() noexcept
    : r(0.f)
    , g(0.f)
    , b(0.f)
    , a(0.f)
{
}

constexpr Color::Color(float32 r, float32 g, float32 b, float32 a) noexcept
    : r(r)
    , g(g)
    , b(b)
    , a(a)
{
}

constexpr Color::Color(Float32ConvertibleT auto const val) noexcept
    : r(static_cast<float32>(val))
    , g(static_cast<float32>(val))
    , b(static_cast<float32>(val))
    , a(static_cast<float32>(val))
{
}

constexpr Color& Color::operator+=(Color const& other) noexcept
{
    r += other.r;
    g += other.g;
    b += other.b;
    a += other.a;
    return *this;
}

constexpr Color& Color::operator-=(Color const& other) noexcept
{
    r -= other.r;
    g -= other.g;
    b -= other.b;
    a -= other.a;
    return *this;
}

constexpr Color& Color::operator*=(Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    r *= fVal;
    g *= fVal;
    b *= fVal;
    a *= fVal;
    return *this;
}

constexpr Color& Color::operator/=(Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    r /= fVal;
    g /= fVal;
    b /= fVal;
    a /= fVal;
    return *this;
}

constexpr Color Color::operator+() const noexcept
{
    return { r, g, b, a };
}

constexpr Color Color::operator-() const noexcept
{
    return { -r, -g, -b, -a };
}

constexpr bool Color::operator==(Color const& other) const noexcept
{
    return r == other.r && g == other.g && b == other.b && a == other.a;
}

constexpr Color operator+(Color const& lhs, Color const& rhs) noexcept
{
    Color result;
    result.r = lhs.r + rhs.r;
    result.g = lhs.g + rhs.g;
    result.b = lhs.b + rhs.b;
    result.a = lhs.a + rhs.a;
    return result;
}

constexpr Color operator-(Color const& lhs, Color const& rhs) noexcept
{
    Color result;
    result.r = lhs.r - rhs.r;
    result.g = lhs.g - rhs.g;
    result.b = lhs.b - rhs.b;
    result.a = lhs.a - rhs.a;
    return result;
}

constexpr Color operator*(Color const& color, Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Color result;
    result.r = color.r * fVal;
    result.g = color.g * fVal;
    result.b = color.b * fVal;
    result.a = color.a * fVal;
    return result;
}

constexpr Color operator/(Color const& color, Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Color result;
    result.r = color.r / fVal;
    result.g = color.g / fVal;
    result.b = color.b / fVal;
    result.a = color.a / fVal;
    return result;
}

constexpr Color operator*(Float32ConvertibleT auto val, Color const& color) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Color result;
    result.r = color.r * fVal;
    result.g = color.g * fVal;
    result.b = color.b * fVal;
    result.a = color.a * fVal;
    return result;
}

} // namespace mini