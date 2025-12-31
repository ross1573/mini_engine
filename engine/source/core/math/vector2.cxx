export module mini.core:vector2;

import :type;
import :math_type;
import :math_operation;

namespace mini {

export struct Vector2 {
public:
    union {
        struct {
            float32 x;
            float32 y;
        };

        float32 data[2];
    };

    constexpr Vector2() noexcept;
    constexpr Vector2(float32, float32) noexcept;
    explicit constexpr Vector2(Float32ConvertibleT auto) noexcept;

    /*constexpr*/ float32 Length() const noexcept;
    /*constexpr*/ float32 LengthSquared() const noexcept;

    constexpr float32 Dot(Vector2 const&) const noexcept;
    constexpr float32 Cross(Vector2 const&) const noexcept;

    constexpr void Clamp(Vector2 const&, Vector2 const&) noexcept;
    /*constexpr*/ void Normalize() noexcept;

    static constexpr float32 Dot(Vector2 const&, Vector2 const&) noexcept;
    static constexpr float32 Cross(Vector2 const&, Vector2 const&) noexcept;

    static /*constexpr*/ Vector2 Normalize(Vector2 const&) noexcept;

    static /*constexpr*/ float32 Distance(Vector2 const&, Vector2 const&) noexcept;
    static /*constexpr*/ float32 DistanceSquared(Vector2 const&, Vector2 const&) noexcept;
    static constexpr Vector2 Lerp(Vector2 const&, Vector2 const&, float32) noexcept;

    constexpr Vector2& operator+=(Vector2 const&) noexcept;
    constexpr Vector2& operator-=(Vector2 const&) noexcept;
    constexpr Vector2& operator*=(Float32ConvertibleT auto) noexcept;
    constexpr Vector2& operator/=(Float32ConvertibleT auto) noexcept;

    constexpr Vector2 operator+() const noexcept;
    constexpr Vector2 operator-() const noexcept;

    constexpr bool operator==(Vector2 const&) const noexcept;

    static constexpr Vector2 Zero() noexcept { return Vector2(0.f, 0.f); }
    static constexpr Vector2 One() noexcept { return Vector2(1.f, 1.f); }
    static constexpr Vector2 UnitX() noexcept { return Vector2(1.f, 0.f); }
    static constexpr Vector2 UnitY() noexcept { return Vector2(0.f, 1.f); }
};

export constexpr Vector2 operator+(Vector2 const&, Vector2 const&) noexcept;
export constexpr Vector2 operator-(Vector2 const&, Vector2 const&) noexcept;
export constexpr Vector2 operator*(Vector2 const&, Float32ConvertibleT auto const) noexcept;
export constexpr Vector2 operator/(Vector2 const&, Float32ConvertibleT auto const) noexcept;
export constexpr Vector2 operator*(Float32ConvertibleT auto const, Vector2 const&) noexcept;

inline constexpr Vector2::Vector2() noexcept
    : x(0)
    , y(0)
{
}

inline constexpr Vector2::Vector2(float32 inX, float32 inY) noexcept
    : x(inX)
    , y(inY)
{
}

inline constexpr Vector2::Vector2(Float32ConvertibleT auto val) noexcept
    : x(static_cast<float32>(val))
    , y(static_cast<float32>(val))
{
}

inline /*constexpr*/ float32 Vector2::Length() const noexcept
{
    return Sqrt(LengthSquared());
}

inline /*constexpr*/ float32 Vector2::LengthSquared() const noexcept
{
    return Pow(x, 2.f) + Pow(y, 2.f);
}

inline constexpr float32 Vector2::Dot(Vector2 const& v) const noexcept
{
    return Vector2::Dot(*this, v);
}

inline constexpr float32 Vector2::Cross(Vector2 const& v) const noexcept
{
    return Vector2::Cross(*this, v);
}

inline constexpr void Vector2::Clamp(Vector2 const& v1, Vector2 const& v2) noexcept
{
    x = (x < v1.x) ? v1.x : ((x > v2.x) ? v2.x : x);
    y = (y < v1.y) ? v1.y : ((y > v2.y) ? v2.y : y);
}

inline /*constexpr*/ void Vector2::Normalize() noexcept
{
    (*this) /= Length();
}

inline constexpr float32 Vector2::Dot(Vector2 const& v1, Vector2 const& v2) noexcept
{
    return (v1.x * v2.x) + (v1.y * v2.y);
}

inline constexpr float32 Vector2::Cross(Vector2 const& v1, Vector2 const& v2) noexcept
{
    return (v1.x * v2.y) - (v1.y * v2.x);
}

inline /*constexpr*/ Vector2 Vector2::Normalize(Vector2 const& v) noexcept
{
    return v / v.Length();
}

inline /*constexpr*/ float32 Vector2::Distance(Vector2 const& v1, Vector2 const& v2) noexcept
{
    return (v1 - v2).Length();
}

inline /*constexpr*/ float32 Vector2::DistanceSquared(Vector2 const& v1, Vector2 const& v2) noexcept
{
    return (v1 - v2).LengthSquared();
}

inline constexpr Vector2 Vector2::Lerp(Vector2 const& v1, Vector2 const& v2, float32 t) noexcept
{
    return (v2 - v1) * t + v1;
}

inline constexpr Vector2& Vector2::operator+=(Vector2 const& v) noexcept
{
    x += v.x;
    y += v.y;
    return *this;
}

inline constexpr Vector2& Vector2::operator-=(Vector2 const& v) noexcept
{
    x -= v.x;
    y -= v.y;
    return *this;
}

inline constexpr Vector2& Vector2::operator*=(Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    x *= fVal;
    y *= fVal;
    return *this;
}

inline constexpr Vector2& Vector2::operator/=(Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    x /= fVal;
    y /= fVal;
    return *this;
}

inline constexpr Vector2 Vector2::operator+() const noexcept
{
    return Vector2(x, y);
}

inline constexpr Vector2 Vector2::operator-() const noexcept
{
    return Vector2(-x, -y);
}

inline constexpr bool Vector2::operator==(Vector2 const& v) const noexcept
{
    return x == v.x && y == v.y;
}

inline constexpr Vector2 operator+(Vector2 const& v1, Vector2 const& v2) noexcept
{
    Vector2 result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    return result;
}

inline constexpr Vector2 operator-(Vector2 const& v1, Vector2 const& v2) noexcept
{
    Vector2 result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    return result;
}

inline constexpr Vector2 operator*(Vector2 const& v, Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Vector2 result;
    result.x = v.x * fVal;
    result.y = v.y * fVal;
    return result;
}

inline constexpr Vector2 operator/(Vector2 const& v, Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Vector2 result;
    result.x = v.x / fVal;
    result.y = v.y / fVal;
    return result;
}

inline constexpr Vector2 operator*(Float32ConvertibleT auto val, Vector2 const& v) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Vector2 result;
    result.x = v.x * fVal;
    result.y = v.y * fVal;
    return result;
}

} // namespace mini