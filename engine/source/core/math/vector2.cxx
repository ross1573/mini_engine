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
    constexpr Vector2(float32 x, float32 y) noexcept;
    explicit constexpr Vector2(Float32ConvertibleT auto val) noexcept;

    [[nodiscard]] /*constexpr*/ float32 Length() const noexcept;
    [[nodiscard]] /*constexpr*/ float32 LengthSquared() const noexcept;

    [[nodiscard]] constexpr float32 Dot(Vector2 const& vec) const noexcept;
    [[nodiscard]] constexpr float32 Cross(Vector2 const& vec) const noexcept;

    constexpr void Clamp(Vector2 const& min, Vector2 const& max) noexcept;
    /*constexpr*/ void Normalize() noexcept;

    static constexpr float32 Dot(Vector2 const& lhs, Vector2 const& rhs) noexcept;
    static constexpr float32 Cross(Vector2 const& lhs, Vector2 const& rhs) noexcept;

    static /*constexpr*/ Vector2 Normalize(Vector2 const& vec) noexcept;

    static /*constexpr*/ float32 Distance(Vector2 const& lhs, Vector2 const& rhs) noexcept;
    static /*constexpr*/ float32 DistanceSquared(Vector2 const& lhs, Vector2 const& rhs) noexcept;
    static constexpr Vector2 Lerp(Vector2 const& x, Vector2 const& y, float32 ratio) noexcept;

    constexpr Vector2& operator+=(Vector2 const& vec) noexcept;
    constexpr Vector2& operator-=(Vector2 const& vec) noexcept;
    constexpr Vector2& operator*=(Float32ConvertibleT auto val) noexcept;
    constexpr Vector2& operator/=(Float32ConvertibleT auto val) noexcept;

    constexpr Vector2 operator+() const noexcept;
    constexpr Vector2 operator-() const noexcept;

    constexpr bool operator==(Vector2 const& other) const noexcept;

    static constexpr Vector2 Zero() noexcept { return { 0.f, 0.f }; }
    static constexpr Vector2 One() noexcept { return { 1.f, 1.f }; }
    static constexpr Vector2 UnitX() noexcept { return { 1.f, 0.f }; }
    static constexpr Vector2 UnitY() noexcept { return { 0.f, 1.f }; }
};

export constexpr Vector2 operator+(Vector2 const& lhs, Vector2 const& rhs) noexcept;
export constexpr Vector2 operator-(Vector2 const& lhs, Vector2 const& rhs) noexcept;
export constexpr Vector2 operator*(Vector2 const& vec, Float32ConvertibleT auto val) noexcept;
export constexpr Vector2 operator/(Vector2 const& vec, Float32ConvertibleT auto val) noexcept;
export constexpr Vector2 operator*(Float32ConvertibleT auto val, Vector2 const& vec) noexcept;

constexpr Vector2::Vector2() noexcept
    : x(0)
    , y(0)
{
}

constexpr Vector2::Vector2(float32 x, float32 y) noexcept
    : x(x)
    , y(y)
{
}

constexpr Vector2::Vector2(Float32ConvertibleT auto val) noexcept
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

constexpr float32 Vector2::Dot(Vector2 const& vec) const noexcept
{
    return Vector2::Dot(*this, vec);
}

constexpr float32 Vector2::Cross(Vector2 const& vec) const noexcept
{
    return Vector2::Cross(*this, vec);
}

constexpr void Vector2::Clamp(Vector2 const& min, Vector2 const& max) noexcept
{
    x = (x < min.x) ? min.x : ((x > max.x) ? max.x : x);
    y = (y < min.y) ? min.y : ((y > max.y) ? max.y : y);
}

inline /*constexpr*/ void Vector2::Normalize() noexcept
{
    (*this) /= Length();
}

constexpr float32 Vector2::Dot(Vector2 const& lhs, Vector2 const& rhs) noexcept
{
    return (lhs.x * rhs.x) + (lhs.y * rhs.y);
}

constexpr float32 Vector2::Cross(Vector2 const& lhs, Vector2 const& rhs) noexcept
{
    return (lhs.x * rhs.y) - (lhs.y * rhs.x);
}

inline /*constexpr*/ Vector2 Vector2::Normalize(Vector2 const& vec) noexcept
{
    return vec / vec.Length();
}

inline /*constexpr*/ float32 Vector2::Distance(Vector2 const& lhs, Vector2 const& rhs) noexcept
{
    return (lhs - rhs).Length();
}

inline /*constexpr*/ float32 Vector2::DistanceSquared(Vector2 const& lhs, Vector2 const& rhs) noexcept
{
    return (lhs - rhs).LengthSquared();
}

constexpr Vector2 Vector2::Lerp(Vector2 const& x, Vector2 const& y, float32 ratio) noexcept
{
    return (y - x) * ratio + x;
}

constexpr Vector2& Vector2::operator+=(Vector2 const& vec) noexcept
{
    x += vec.x;
    y += vec.y;
    return *this;
}

constexpr Vector2& Vector2::operator-=(Vector2 const& vec) noexcept
{
    x -= vec.x;
    y -= vec.y;
    return *this;
}

constexpr Vector2& Vector2::operator*=(Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    x *= fVal;
    y *= fVal;
    return *this;
}

constexpr Vector2& Vector2::operator/=(Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    x /= fVal;
    y /= fVal;
    return *this;
}

constexpr Vector2 Vector2::operator+() const noexcept
{
    return { x, y };
}

constexpr Vector2 Vector2::operator-() const noexcept
{
    return { -x, -y };
}

constexpr bool Vector2::operator==(Vector2 const& other) const noexcept
{
    return x == other.x && y == other.y;
}

constexpr Vector2 operator+(Vector2 const& lhs, Vector2 const& rhs) noexcept
{
    Vector2 result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    return result;
}

constexpr Vector2 operator-(Vector2 const& lhs, Vector2 const& rhs) noexcept
{
    Vector2 result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    return result;
}

constexpr Vector2 operator*(Vector2 const& vec, Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Vector2 result;
    result.x = vec.x * fVal;
    result.y = vec.y * fVal;
    return result;
}

constexpr Vector2 operator/(Vector2 const& vec, Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Vector2 result;
    result.x = vec.x / fVal;
    result.y = vec.y / fVal;
    return result;
}

constexpr Vector2 operator*(Float32ConvertibleT auto val, Vector2 const& vec) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Vector2 result;
    result.x = vec.x * fVal;
    result.y = vec.y * fVal;
    return result;
}

} // namespace mini