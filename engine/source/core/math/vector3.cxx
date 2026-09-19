export module mini.core:vector3;

import :type;
import :math_type;
import :math_operation;
import :vector2;

namespace mini {

export struct Vector3 {
public:
    union {
        struct {
            float32 x;
            float32 y;
            float32 z;
        };

        float32 data[3];
    };

    constexpr Vector3() noexcept;
    constexpr Vector3(float32 x, float32 y, float32 z) noexcept;
    explicit constexpr Vector3(Float32ConvertibleT auto val) noexcept;

    [[nodiscard]] /*constexpr*/ float32 Length() const noexcept;
    [[nodiscard]] /*constexpr*/ float32 LengthSquared() const noexcept;

    [[nodiscard]] constexpr float32 Dot(Vector3 const& vec) const noexcept;
    [[nodiscard]] constexpr Vector3 Cross(Vector3 const& vec) const noexcept;

    constexpr void Clamp(Vector3 const& min, Vector3 const& max) noexcept;
    /*constexpr*/ void Normalize() noexcept;

    static constexpr float32 Dot(Vector3 const& lhs, Vector3 const& rhs) noexcept;
    static constexpr Vector3 Cross(Vector3 const& lhs, Vector3 const& rhs) noexcept;

    static /*constexpr*/ Vector3 Normalize(Vector3 const& vec) noexcept;

    static /*constexpr*/ float32 Distance(Vector3 const& lhs, Vector3 const& rhs) noexcept;
    static /*constexpr*/ float32 DistanceSquared(Vector3 const& lhs, Vector3 const& rhs) noexcept;
    static constexpr Vector3 Lerp(Vector3 const& x, Vector3 const& y, float32 ratio) noexcept;

    constexpr Vector3& operator+=(Vector3 const& vec) noexcept;
    constexpr Vector3& operator-=(Vector3 const& vec) noexcept;
    constexpr Vector3& operator*=(Float32ConvertibleT auto val) noexcept;
    constexpr Vector3& operator/=(Float32ConvertibleT auto val) noexcept;

    constexpr Vector3 operator+() const noexcept;
    constexpr Vector3 operator-() const noexcept;

    constexpr bool operator==(Vector3 const& other) const noexcept;

    explicit constexpr operator Vector2() const noexcept;

    static constexpr Vector3 Zero() noexcept { return { 0.f, 0.f, 0.f }; }
    static constexpr Vector3 One() noexcept { return { 1.f, 1.f, 1.f }; }
    static constexpr Vector3 UnitX() noexcept { return { 1.f, 0.f, 0.f }; }
    static constexpr Vector3 UnitY() noexcept { return { 0.f, 1.f, 0.f }; }
    static constexpr Vector3 UnitZ() noexcept { return { 0.f, 0.f, 1.f }; }
    static constexpr Vector3 Up() noexcept { return { 0.f, 1.f, 0.f }; }
    static constexpr Vector3 Down() noexcept { return { 0.f, -1.f, 0.f }; }
    static constexpr Vector3 Left() noexcept { return { -1.f, 0.f, 0.f }; }
    static constexpr Vector3 Right() noexcept { return { 1.f, 0.f, 0.f }; }
    static constexpr Vector3 Forward() noexcept { return { 0.f, 0.f, 1.f }; }
    static constexpr Vector3 Backward() noexcept { return { 0.f, 0.f, -1.f }; }
};

export constexpr Vector3 operator+(Vector3 const& lhs, Vector3 const& rhs) noexcept;
export constexpr Vector3 operator-(Vector3 const& lhs, Vector3 const& rhs) noexcept;
export constexpr Vector3 operator*(Vector3 const& vec, Float32ConvertibleT auto val) noexcept;
export constexpr Vector3 operator/(Vector3 const& vec, Float32ConvertibleT auto val) noexcept;
export constexpr Vector3 operator*(Float32ConvertibleT auto val, Vector3 const& vec) noexcept;

constexpr Vector3::Vector3() noexcept
    : x(0.f)
    , y(0.f)
    , z(0.f)
{
}

constexpr Vector3::Vector3(float32 inX, float32 inY, float32 inZ) noexcept
    : x(inX)
    , y(inY)
    , z(inZ)
{
}

constexpr Vector3::Vector3(Float32ConvertibleT auto val) noexcept
    : x(static_cast<float32>(val))
    , y(static_cast<float32>(val))
    , z(static_cast<float32>(val))
{
}

inline /*constexpr*/ float32 Vector3::Length() const noexcept
{
    return Sqrt(LengthSquared());
}

inline /*constexpr*/ float32 Vector3::LengthSquared() const noexcept
{
    return Pow(x, 2.f) + Pow(y, 2.f) + Pow(z, 2.f);
}

constexpr float32 Vector3::Dot(Vector3 const& vec) const noexcept
{
    return Vector3::Dot(*this, vec);
}

constexpr Vector3 Vector3::Cross(Vector3 const& vec) const noexcept
{
    return Vector3::Cross(*this, vec);
}

constexpr void Vector3::Clamp(Vector3 const& min, Vector3 const& max) noexcept
{
    x = (x < min.x) ? min.x : ((x > max.x) ? max.x : x);
    y = (y < min.y) ? min.y : ((y > max.y) ? max.y : y);
    z = (z < min.z) ? min.z : ((z > max.z) ? max.z : z);
}

inline /*constexpr*/ void Vector3::Normalize() noexcept
{
    (*this) /= Length();
}

constexpr float32 Vector3::Dot(Vector3 const& lhs, Vector3 const& rhs) noexcept
{
    return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

constexpr Vector3 Vector3::Cross(Vector3 const& lhs, Vector3 const& rhs) noexcept
{
    Vector3 result;
    result.x = (lhs.y * rhs.z) - (lhs.z * rhs.y);
    result.y = (lhs.z * rhs.x) - (lhs.x * rhs.z);
    result.z = (lhs.x * rhs.y) - (lhs.y * rhs.x);
    return result;
}

inline /*constexpr*/ Vector3 Vector3::Normalize(Vector3 const& vec) noexcept
{
    return vec / vec.Length();
}

inline /*constexpr*/ float32 Vector3::Distance(Vector3 const& lhs, Vector3 const& rhs) noexcept
{
    return (lhs - rhs).Length();
}

inline /*constexpr*/ float32 Vector3::DistanceSquared(Vector3 const& lhs, Vector3 const& rhs) noexcept
{
    return (lhs - rhs).LengthSquared();
}

constexpr Vector3 Vector3::Lerp(Vector3 const& x, Vector3 const& y, float32 ratio) noexcept
{
    return (y - x) * ratio + x;
}

constexpr Vector3& Vector3::operator+=(Vector3 const& vec) noexcept
{
    x += vec.x;
    y += vec.y;
    z += vec.z;
    return *this;
}

constexpr Vector3& Vector3::operator-=(Vector3 const& vec) noexcept
{
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;
    return *this;
}

constexpr Vector3& Vector3::operator*=(Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    x *= fVal;
    y *= fVal;
    z *= fVal;
    return *this;
}

constexpr Vector3& Vector3::operator/=(Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    x /= fVal;
    y /= fVal;
    z /= fVal;
    return *this;
}

constexpr Vector3 Vector3::operator+() const noexcept
{
    return { x, y, z };
}

constexpr Vector3 Vector3::operator-() const noexcept
{
    return { -x, -y, -z };
}

constexpr bool Vector3::operator==(Vector3 const& other) const noexcept
{
    return x == other.x && y == other.y && z == other.z;
}

constexpr Vector3::operator Vector2() const noexcept
{
    return { x, y };
}

constexpr Vector3 operator+(Vector3 const& lhs, Vector3 const& rhs) noexcept
{
    Vector3 result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    result.z = lhs.z + rhs.z;
    return result;
}

constexpr Vector3 operator-(Vector3 const& lhs, Vector3 const& rhs) noexcept
{
    Vector3 result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    result.z = lhs.z - rhs.z;
    return result;
}

constexpr Vector3 operator*(Vector3 const& vec, Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Vector3 result;
    result.x = vec.x * fVal;
    result.y = vec.y * fVal;
    result.z = vec.z * fVal;
    return result;
}

constexpr Vector3 operator/(Vector3 const& vec, Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Vector3 result;
    result.x = vec.x / fVal;
    result.y = vec.y / fVal;
    result.z = vec.z / fVal;
    return result;
}

constexpr Vector3 operator*(Float32ConvertibleT auto val, Vector3 const& vec) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Vector3 result;
    result.x = vec.x * fVal;
    result.y = vec.y * fVal;
    result.z = vec.z * fVal;
    return result;
}

} // namespace mini