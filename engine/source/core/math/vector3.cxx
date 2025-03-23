export module mini.core:vector3;

import :type;
import :math;
import :vector2;

export namespace mini {

struct Vector3 {
    union {
        struct {
            float32 x;
            float32 y;
            float32 z;
        };

        float32 data[3];
    };

    constexpr Vector3() noexcept;
    constexpr Vector3(float32, float32, float32) noexcept;
    explicit constexpr Vector3(Float32ConvertibleT auto) noexcept;

    /*constexpr*/ float32 Length() const noexcept;
    /*constexpr*/ float32 LengthSquared() const noexcept;

    constexpr float32 Dot(Vector3 const&) const noexcept;
    constexpr Vector3 Cross(Vector3 const&) const noexcept;

    constexpr void Clamp(Vector3 const&, Vector3 const&) noexcept;
    /*constexpr*/ void Normalize() noexcept;

    static constexpr float32 Dot(Vector3 const&, Vector3 const&) noexcept;
    static constexpr Vector3 Cross(Vector3 const&, Vector3 const&) noexcept;

    static /*constexpr*/ Vector3 Normalize(Vector3 const&) noexcept;

    static /*constexpr*/ float32 Distance(Vector3 const&, Vector3 const&) noexcept;
    static /*constexpr*/ float32 DistanceSquared(Vector3 const&, Vector3 const&) noexcept;
    static constexpr Vector3 Lerp(Vector3 const&, Vector3 const&, float32) noexcept;

    constexpr Vector3& operator+=(Vector3 const&) noexcept;
    constexpr Vector3& operator-=(Vector3 const&) noexcept;
    constexpr Vector3& operator*=(Float32ConvertibleT auto) noexcept;
    constexpr Vector3& operator/=(Float32ConvertibleT auto) noexcept;

    constexpr Vector3 operator+() const noexcept;
    constexpr Vector3 operator-() const noexcept;

    constexpr bool operator==(Vector3 const&) const noexcept;

    explicit constexpr operator Vector2() const noexcept;

    static const Vector3 zero;
    static const Vector3 one;
    static const Vector3 unitX;
    static const Vector3 unitY;
    static const Vector3 unitZ;
    static const Vector3 up;
    static const Vector3 down;
    static const Vector3 left;
    static const Vector3 right;
    static const Vector3 forward;
    static const Vector3 backward;
};

constexpr Vector3 operator+(Vector3 const&, Vector3 const&) noexcept;
constexpr Vector3 operator-(Vector3 const&, Vector3 const&) noexcept;
constexpr Vector3 operator*(Vector3 const&, Float32ConvertibleT auto const) noexcept;
constexpr Vector3 operator/(Vector3 const&, Float32ConvertibleT auto const) noexcept;
constexpr Vector3 operator*(Float32ConvertibleT auto const, Vector3 const&) noexcept;

inline Vector3 const Vector3::zero = Vector3();
inline Vector3 const Vector3::one = Vector3(1.f, 1.f, 1.f);
inline Vector3 const Vector3::unitX = Vector3(1.f, 0.f, 0.f);
inline Vector3 const Vector3::unitY = Vector3(0.f, 1.f, 0.f);
inline Vector3 const Vector3::unitZ = Vector3(0.f, 0.f, 1.f);
inline Vector3 const Vector3::up = Vector3(0.f, 1.f, 0.f);
inline Vector3 const Vector3::down = Vector3(0.f, -1.f, 0.f);
inline Vector3 const Vector3::left = Vector3(-1.f, 0.f, 0.f);
inline Vector3 const Vector3::right = Vector3(1.f, 0.f, 0.f);
inline Vector3 const Vector3::forward = Vector3(0.f, 0.f, 1.f);
inline Vector3 const Vector3::backward = Vector3(0.f, 0.f, -1.f);

inline constexpr Vector3::Vector3() noexcept
    : x(0.f)
    , y(0.f)
    , z(0.f)
{
}

inline constexpr Vector3::Vector3(float32 inX, float32 inY, float32 inZ) noexcept
    : x(inX)
    , y(inY)
    , z(inZ)
{
}

inline constexpr Vector3::Vector3(Float32ConvertibleT auto val) noexcept
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

inline constexpr float32 Vector3::Dot(Vector3 const& v) const noexcept
{
    return Vector3::Dot(*this, v);
}

inline constexpr Vector3 Vector3::Cross(Vector3 const& v) const noexcept
{
    return Vector3::Cross(*this, v);
}

inline constexpr void Vector3::Clamp(Vector3 const& v1, Vector3 const& v2) noexcept
{
    x = (x < v1.x) ? v1.x : ((x > v2.x) ? v2.x : x);
    y = (y < v1.y) ? v1.y : ((y > v2.y) ? v2.y : y);
    z = (z < v1.z) ? v1.z : ((z > v2.z) ? v2.z : z);
}

inline /*constexpr*/ void Vector3::Normalize() noexcept
{
    (*this) /= Length();
}

inline constexpr float32 Vector3::Dot(Vector3 const& v1, Vector3 const& v2) noexcept
{
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

inline constexpr Vector3 Vector3::Cross(Vector3 const& v1, Vector3 const& v2) noexcept
{
    Vector3 result;
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;
    return result;
}

inline /*constexpr*/ Vector3 Vector3::Normalize(Vector3 const& v) noexcept
{
    return v / v.Length();
}

inline /*constexpr*/ float32 Vector3::Distance(Vector3 const& v1, Vector3 const& v2) noexcept
{
    return (v1 - v2).Length();
}

inline /*constexpr*/ float32 Vector3::DistanceSquared(Vector3 const& v1, Vector3 const& v2) noexcept
{
    return (v1 - v2).LengthSquared();
}

inline constexpr Vector3 Vector3::Lerp(Vector3 const& v1, Vector3 const& v2, float32 t) noexcept
{
    return (v2 - v1) * t + v1;
}

inline constexpr Vector3& Vector3::operator+=(Vector3 const& v) noexcept
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

inline constexpr Vector3& Vector3::operator-=(Vector3 const& v) noexcept
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

inline constexpr Vector3& Vector3::operator*=(Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    x *= fVal;
    y *= fVal;
    z *= fVal;
    return *this;
}

inline constexpr Vector3& Vector3::operator/=(Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    x /= fVal;
    y /= fVal;
    z /= fVal;
    return *this;
}

inline constexpr Vector3 Vector3::operator+() const noexcept
{
    return Vector3(x, y, z);
}

inline constexpr Vector3 Vector3::operator-() const noexcept
{
    return Vector3(-x, -y, -z);
}

inline constexpr bool Vector3::operator==(Vector3 const& v) const noexcept
{
    return x == v.x && y == v.y && z == v.z;
}

inline constexpr Vector3::operator Vector2() const noexcept
{
    return Vector2(x, y);
}

inline constexpr Vector3 operator+(Vector3 const& v1, Vector3 const& v2) noexcept
{
    Vector3 result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    result.z = v1.z + v2.z;
    return result;
}

inline constexpr Vector3 operator-(Vector3 const& v1, Vector3 const& v2) noexcept
{
    Vector3 result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
    return result;
}

inline constexpr Vector3 operator*(Vector3 const& v, Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Vector3 result;
    result.x = v.x * fVal;
    result.y = v.y * fVal;
    result.z = v.z * fVal;
    return result;
}

inline constexpr Vector3 operator/(Vector3 const& v, Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Vector3 result;
    result.x = v.x / fVal;
    result.y = v.y / fVal;
    result.z = v.z / fVal;
    return result;
}

inline constexpr Vector3 operator*(Float32ConvertibleT auto val, Vector3 const& v) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Vector3 result;
    result.x = v.x * fVal;
    result.y = v.y * fVal;
    result.z = v.z * fVal;
    return result;
}

} // namespace mini