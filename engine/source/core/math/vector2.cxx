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

    [[nodiscard]] /*constexpr*/ float32 length() const noexcept;
    [[nodiscard]] /*constexpr*/ float32 lengthSquared() const noexcept;

    [[nodiscard]] constexpr float32 dot(Vector2 const& vec) const noexcept;
    [[nodiscard]] constexpr float32 cross(Vector2 const& vec) const noexcept;

    constexpr void clamp(Vector2 const& min, Vector2 const& max) noexcept;
    /*constexpr*/ void normalize() noexcept;

    static constexpr float32 dot(Vector2 const& lhs, Vector2 const& rhs) noexcept;
    static constexpr float32 cross(Vector2 const& lhs, Vector2 const& rhs) noexcept;

    static /*constexpr*/ Vector2 normalize(Vector2 const& vec) noexcept;

    static /*constexpr*/ float32 distance(Vector2 const& lhs, Vector2 const& rhs) noexcept;
    static /*constexpr*/ float32 distanceSquared(Vector2 const& lhs, Vector2 const& rhs) noexcept;
    static constexpr Vector2 lerp(Vector2 const& x, Vector2 const& y, float32 ratio) noexcept;

    constexpr Vector2& operator+=(Vector2 const& vec) noexcept;
    constexpr Vector2& operator-=(Vector2 const& vec) noexcept;
    constexpr Vector2& operator*=(Float32ConvertibleT auto val) noexcept;
    constexpr Vector2& operator/=(Float32ConvertibleT auto val) noexcept;

    constexpr Vector2 operator+() const noexcept;
    constexpr Vector2 operator-() const noexcept;

    constexpr bool operator==(Vector2 const& other) const noexcept;

    static constexpr Vector2 zero() noexcept { return {0.f, 0.f}; }
    static constexpr Vector2 one() noexcept { return {1.f, 1.f}; }
    static constexpr Vector2 unitX() noexcept { return {1.f, 0.f}; }
    static constexpr Vector2 unitY() noexcept { return {0.f, 1.f}; }
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

inline /*constexpr*/ float32 Vector2::length() const noexcept
{
    return sqrt(lengthSquared());
}

inline /*constexpr*/ float32 Vector2::lengthSquared() const noexcept
{
    return pow(x, 2.f) + pow(y, 2.f);
}

constexpr float32 Vector2::dot(Vector2 const& vec) const noexcept
{
    return Vector2::dot(*this, vec);
}

constexpr float32 Vector2::cross(Vector2 const& vec) const noexcept
{
    return Vector2::cross(*this, vec);
}

constexpr void Vector2::clamp(Vector2 const& min, Vector2 const& max) noexcept
{
    x = (x < min.x) ? min.x : ((x > max.x) ? max.x : x);
    y = (y < min.y) ? min.y : ((y > max.y) ? max.y : y);
}

inline /*constexpr*/ void Vector2::normalize() noexcept
{
    (*this) /= length();
}

constexpr float32 Vector2::dot(Vector2 const& lhs, Vector2 const& rhs) noexcept
{
    return (lhs.x * rhs.x) + (lhs.y * rhs.y);
}

constexpr float32 Vector2::cross(Vector2 const& lhs, Vector2 const& rhs) noexcept
{
    return (lhs.x * rhs.y) - (lhs.y * rhs.x);
}

inline /*constexpr*/ Vector2 Vector2::normalize(Vector2 const& vec) noexcept
{
    return vec / vec.length();
}

inline /*constexpr*/ float32 Vector2::distance(Vector2 const& lhs, Vector2 const& rhs) noexcept
{
    return (lhs - rhs).length();
}

inline /*constexpr*/ float32 Vector2::distanceSquared(Vector2 const& lhs, Vector2 const& rhs) noexcept
{
    return (lhs - rhs).lengthSquared();
}

constexpr Vector2 Vector2::lerp(Vector2 const& x, Vector2 const& y, float32 ratio) noexcept
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
    return {x, y};
}

constexpr Vector2 Vector2::operator-() const noexcept
{
    return {-x, -y};
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