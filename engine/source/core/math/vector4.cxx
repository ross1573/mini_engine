export module mini.core:vector4;

import :type;
import :math;
import :vector3;

export namespace mini {

struct Vector4 {
    union {
        struct {
            float32 x;
            float32 y;
            float32 z;
            float32 w;
        };

        float32 data[4];
    };

    constexpr Vector4() noexcept;
    constexpr Vector4(float, float, float, float) noexcept;
    explicit constexpr Vector4(Float32ConvertibleT auto) noexcept;

    /*constexpr*/ float Length() const noexcept;
    /*constexpr*/ float LengthSquared() const noexcept;

    constexpr float Dot(Vector4 const&) const noexcept;
    constexpr Vector4 Cross(Vector4 const&) const noexcept;

    constexpr void Clamp(Vector4 const&, Vector4 const&) noexcept;
    /*constexpr*/ void Normalize() noexcept;

    static constexpr float Dot(Vector4 const&, Vector4 const&) noexcept;
    static constexpr Vector4 Cross(Vector4 const&, Vector4 const&) noexcept;

    static /*constexpr*/ Vector4 Normalize(Vector4 const&) noexcept;

    static /*constexpr*/ float Distance(Vector4 const&, Vector4 const&) noexcept;
    static /*constexpr*/ float DistanceSquared(Vector4 const&, Vector4 const&) noexcept;
    static constexpr Vector4 Lerp(Vector4 const&, Vector4 const&, float) noexcept;

    constexpr Vector4& operator+=(Vector4 const&) noexcept;
    constexpr Vector4& operator-=(Vector4 const&) noexcept;
    constexpr Vector4& operator*=(Float32ConvertibleT auto) noexcept;
    constexpr Vector4& operator/=(Float32ConvertibleT auto) noexcept;

    constexpr Vector4 operator+() const noexcept;
    constexpr Vector4 operator-() const noexcept;

    constexpr bool operator==(Vector4 const&) const noexcept;

    constexpr explicit operator Vector3() const noexcept;

    static const Vector4 zero;
    static const Vector4 one;
    static const Vector4 unitX;
    static const Vector4 unitY;
    static const Vector4 unitZ;
    static const Vector4 unitW;
};

constexpr Vector4 operator+(Vector4 const&, Vector4 const&) noexcept;
constexpr Vector4 operator-(Vector4 const&, Vector4 const&) noexcept;
constexpr Vector4 operator*(Vector4 const&, Float32ConvertibleT auto const) noexcept;
constexpr Vector4 operator/(Vector4 const&, Float32ConvertibleT auto const) noexcept;
constexpr Vector4 operator*(Float32ConvertibleT auto const, Vector4 const&) noexcept;

inline Vector4 const Vector4::zero = Vector4();
inline Vector4 const Vector4::one = Vector4(1.f, 1.f, 1.f, 1.f);
inline Vector4 const Vector4::unitX = Vector4(1.f, 0.f, 0.f, 0.f);
inline Vector4 const Vector4::unitY = Vector4(0.f, 1.f, 0.f, 0.f);
inline Vector4 const Vector4::unitZ = Vector4(0.f, 0.f, 1.f, 0.f);
inline Vector4 const Vector4::unitW = Vector4(0.f, 0.f, 0.f, 1.f);

inline constexpr Vector4::Vector4() noexcept
    : x(0.f)
    , y(0.f)
    , z(0.f)
    , w(0.f)
{
}

inline constexpr Vector4::Vector4(float inX, float inY, float inZ, float inW) noexcept
    : x(inX)
    , y(inY)
    , z(inZ)
    , w(inW)
{
}

inline constexpr Vector4::Vector4(Float32ConvertibleT auto val) noexcept
    : x(static_cast<float>(val))
    , y(static_cast<float>(val))
    , z(static_cast<float>(val))
    , w(static_cast<float>(val))
{
}

inline /*constexpr*/ float Vector4::Length() const noexcept
{
    return Sqrt(LengthSquared());
}

inline /*constexpr*/ float Vector4::LengthSquared() const noexcept
{
    return Pow(x, 2.f) + Pow(y, 2.f) + Pow(z, 2.f) + Pow(w, 2.f);
}

inline constexpr float Vector4::Dot(Vector4 const& v) const noexcept
{
    return Vector4::Dot(*this, v);
}

inline constexpr Vector4 Vector4::Cross(Vector4 const& v) const noexcept
{
    return Vector4::Cross(*this, v);
}

inline constexpr void Vector4::Clamp(Vector4 const& v1, Vector4 const& v2) noexcept
{
    x = (x < v1.x) ? v1.x : ((x > v2.x) ? v2.x : x);
    y = (y < v1.y) ? v1.y : ((y > v2.y) ? v2.y : y);
    z = (z < v1.z) ? v1.z : ((z > v2.z) ? v2.z : z);
    w = (w < v1.w) ? v1.w : ((w > v2.w) ? v2.w : w);
}

inline /*constexpr*/ void Vector4::Normalize() noexcept
{
    (*this) /= Length();
}

inline constexpr float Vector4::Dot(Vector4 const& v1, Vector4 const& v2) noexcept
{
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w);
}

inline constexpr Vector4 Vector4::Cross(Vector4 const& v1, Vector4 const& v2) noexcept
{
    Vector4 result;
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;
    result.w = v1.w * v2.w - v2.w * v2.w;
    return result;
}

inline /*constexpr*/ Vector4 Vector4::Normalize(Vector4 const& v) noexcept
{
    return v / v.Length();
}

inline /*constexpr*/ float Vector4::Distance(Vector4 const& v1, Vector4 const& v2) noexcept
{
    return (v1 - v2).Length();
}

inline /*constexpr*/ float Vector4::DistanceSquared(Vector4 const& v1, Vector4 const& v2) noexcept
{
    return (v1 - v2).LengthSquared();
}

inline constexpr Vector4 Vector4::Lerp(Vector4 const& v1, Vector4 const& v2, float t) noexcept
{
    return (v2 - v1) * t + v1;
}

inline constexpr Vector4& Vector4::operator+=(Vector4 const& v) noexcept
{
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
}

inline constexpr Vector4& Vector4::operator-=(Vector4 const& v) noexcept
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
}

inline constexpr Vector4& Vector4::operator*=(Float32ConvertibleT auto val) noexcept
{
    const float fVal = static_cast<float>(val);
    x *= fVal;
    y *= fVal;
    z *= fVal;
    w *= fVal;
    return *this;
}

inline constexpr Vector4& Vector4::operator/=(Float32ConvertibleT auto val) noexcept
{
    const float fVal = static_cast<float>(val);
    x /= fVal;
    y /= fVal;
    z /= fVal;
    w /= fVal;
    return *this;
}

inline constexpr Vector4 Vector4::operator+() const noexcept
{
    return Vector4(x, y, z, w);
}

inline constexpr Vector4 Vector4::operator-() const noexcept
{
    return Vector4(-x, -y, -z, -w);
}

inline constexpr bool Vector4::operator==(Vector4 const& v) const noexcept
{
    return x == v.x && y == v.y && z == v.z && w == v.w;
}

inline constexpr Vector4::operator Vector3() const noexcept
{
    return Vector3(x, y, z);
}

inline constexpr Vector4 operator+(Vector4 const& v1, Vector4 const& v2) noexcept
{
    Vector4 result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    result.z = v1.z + v2.z;
    result.w = v1.w + v2.w;
    return result;
}

inline constexpr Vector4 operator-(Vector4 const& v1, Vector4 const& v2) noexcept
{
    Vector4 result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
    result.w = v1.w - v2.w;
    return result;
}

inline constexpr Vector4 operator*(Vector4 const& v, Float32ConvertibleT auto val) noexcept
{
    const float fVal = static_cast<float>(val);
    Vector4 result;
    result.x = v.x * fVal;
    result.y = v.y * fVal;
    result.z = v.z * fVal;
    result.w = v.w * fVal;
    return result;
}

inline constexpr Vector4 operator/(Vector4 const& v, Float32ConvertibleT auto val) noexcept
{
    const float fVal = static_cast<float>(val);
    Vector4 result;
    result.x = v.x / fVal;
    result.y = v.y / fVal;
    result.z = v.z / fVal;
    result.w = v.w / fVal;
    return result;
}

inline constexpr Vector4 operator*(Float32ConvertibleT auto val, Vector4 const& v) noexcept
{
    const float fVal = static_cast<float>(val);
    Vector4 result;
    result.x = v.x * fVal;
    result.y = v.y * fVal;
    result.z = v.z * fVal;
    result.w = v.w * fVal;
    return result;
}

} // namespace mini