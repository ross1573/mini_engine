export module mini.core:vector4;

import :type;
import :math_type;
import :math_operation;
import :vector3;

namespace mini {

export struct Vector4 {
public:
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
    constexpr Vector4(float32 x, float32 y, float32 z, float32 w) noexcept;
    explicit constexpr Vector4(Float32ConvertibleT auto val) noexcept;

    [[nodiscard]] /*constexpr*/ float32 Length() const noexcept;
    [[nodiscard]] /*constexpr*/ float32 LengthSquared() const noexcept;

    [[nodiscard]] constexpr float32 Dot(Vector4 const& vec) const noexcept;
    [[nodiscard]] constexpr Vector4 Cross(Vector4 const& vec) const noexcept;

    constexpr void Clamp(Vector4 const& min, Vector4 const& max) noexcept;
    /*constexpr*/ void Normalize() noexcept;

    static constexpr float32 Dot(Vector4 const& lhs, Vector4 const& rhs) noexcept;
    static constexpr Vector4 Cross(Vector4 const& lhs, Vector4 const& rhs) noexcept;

    static /*constexpr*/ Vector4 Normalize(Vector4 const& vec) noexcept;

    static /*constexpr*/ float32 Distance(Vector4 const& lhs, Vector4 const& rhs) noexcept;
    static /*constexpr*/ float32 DistanceSquared(Vector4 const& lhs, Vector4 const& rhs) noexcept;
    static constexpr Vector4 Lerp(Vector4 const& x, Vector4 const& y, float32 ratio) noexcept;

    constexpr Vector4& operator+=(Vector4 const& vec) noexcept;
    constexpr Vector4& operator-=(Vector4 const& vec) noexcept;
    constexpr Vector4& operator*=(Float32ConvertibleT auto val) noexcept;
    constexpr Vector4& operator/=(Float32ConvertibleT auto val) noexcept;

    constexpr Vector4 operator+() const noexcept;
    constexpr Vector4 operator-() const noexcept;

    constexpr bool operator==(Vector4 const& other) const noexcept;

    constexpr explicit operator Vector3() const noexcept;

    static constexpr Vector4 Zero() noexcept { return { 0.f, 0.f, 0.f, 0.f }; }
    static constexpr Vector4 One() noexcept { return { 1.f, 1.f, 1.f, 1.f }; }
    static constexpr Vector4 UnitX() noexcept { return { 1.f, 0.f, 0.f, 0.f }; }
    static constexpr Vector4 UnitY() noexcept { return { 0.f, 1.f, 0.f, 0.f }; }
    static constexpr Vector4 UnitZ() noexcept { return { 0.f, 0.f, 1.f, 0.f }; }
    static constexpr Vector4 UnitW() noexcept { return { 0.f, 0.f, 0.f, 1.f }; }
};

export constexpr Vector4 operator+(Vector4 const& lhs, Vector4 const& rhs) noexcept;
export constexpr Vector4 operator-(Vector4 const& lhs, Vector4 const& rhs) noexcept;
export constexpr Vector4 operator*(Vector4 const& vec, Float32ConvertibleT auto val) noexcept;
export constexpr Vector4 operator/(Vector4 const& vec, Float32ConvertibleT auto val) noexcept;
export constexpr Vector4 operator*(Float32ConvertibleT auto val, Vector4 const& vec) noexcept;

constexpr Vector4::Vector4() noexcept
    : x(0.f)
    , y(0.f)
    , z(0.f)
    , w(0.f)
{
}

constexpr Vector4::Vector4(float32 x, float32 y, float32 z, float32 w) noexcept
    : x(x)
    , y(y)
    , z(z)
    , w(w)
{
}

constexpr Vector4::Vector4(Float32ConvertibleT auto val) noexcept
    : x(static_cast<float32>(val))
    , y(static_cast<float32>(val))
    , z(static_cast<float32>(val))
    , w(static_cast<float32>(val))
{
}

inline /*constexpr*/ float32 Vector4::Length() const noexcept
{
    return Sqrt(LengthSquared());
}

inline /*constexpr*/ float32 Vector4::LengthSquared() const noexcept
{
    return Pow(x, 2.f) + Pow(y, 2.f) + Pow(z, 2.f) + Pow(w, 2.f);
}

constexpr float32 Vector4::Dot(Vector4 const& vec) const noexcept
{
    return Vector4::Dot(*this, vec);
}

constexpr Vector4 Vector4::Cross(Vector4 const& vec) const noexcept
{
    return Vector4::Cross(*this, vec);
}

constexpr void Vector4::Clamp(Vector4 const& min, Vector4 const& max) noexcept
{
    x = (x < min.x) ? min.x : ((x > max.x) ? max.x : x);
    y = (y < min.y) ? min.y : ((y > max.y) ? max.y : y);
    z = (z < min.z) ? min.z : ((z > max.z) ? max.z : z);
    w = (w < min.w) ? min.w : ((w > max.w) ? max.w : w);
}

inline /*constexpr*/ void Vector4::Normalize() noexcept
{
    (*this) /= Length();
}

constexpr float32 Vector4::Dot(Vector4 const& lhs, Vector4 const& rhs) noexcept
{
    return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
}

constexpr Vector4 Vector4::Cross(Vector4 const& lhs, Vector4 const& rhs) noexcept
{
    Vector4 result;
    result.x = (lhs.y * rhs.z) - (lhs.z * rhs.y);
    result.y = (lhs.z * rhs.x) - (lhs.x * rhs.z);
    result.z = (lhs.x * rhs.y) - (lhs.y * rhs.x);
    result.w = (lhs.w * rhs.w) - (rhs.w * rhs.w);
    return result;
}

inline /*constexpr*/ Vector4 Vector4::Normalize(Vector4 const& vec) noexcept
{
    return vec / vec.Length();
}

inline /*constexpr*/ float32 Vector4::Distance(Vector4 const& lhs, Vector4 const& rhs) noexcept
{
    return (lhs - rhs).Length();
}

inline /*constexpr*/ float32 Vector4::DistanceSquared(Vector4 const& lhs, Vector4 const& rhs) noexcept
{
    return (lhs - rhs).LengthSquared();
}

constexpr Vector4 Vector4::Lerp(Vector4 const& x, Vector4 const& y, float32 ratio) noexcept
{
    return (y - x) * ratio + x;
}

constexpr Vector4& Vector4::operator+=(Vector4 const& vec) noexcept
{
    x += vec.x;
    y += vec.y;
    z += vec.z;
    w += vec.w;
    return *this;
}

constexpr Vector4& Vector4::operator-=(Vector4 const& vec) noexcept
{
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;
    w -= vec.w;
    return *this;
}

constexpr Vector4& Vector4::operator*=(Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    x *= fVal;
    y *= fVal;
    z *= fVal;
    w *= fVal;
    return *this;
}

constexpr Vector4& Vector4::operator/=(Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    x /= fVal;
    y /= fVal;
    z /= fVal;
    w /= fVal;
    return *this;
}

constexpr Vector4 Vector4::operator+() const noexcept
{
    return { x, y, z, w };
}

constexpr Vector4 Vector4::operator-() const noexcept
{
    return { -x, -y, -z, -w };
}

constexpr bool Vector4::operator==(Vector4 const& other) const noexcept
{
    return x == other.x && y == other.y && z == other.z && w == other.w;
}

constexpr Vector4::operator Vector3() const noexcept
{
    return { x, y, z };
}

constexpr Vector4 operator+(Vector4 const& lhs, Vector4 const& rhs) noexcept
{
    Vector4 result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    result.z = lhs.z + rhs.z;
    result.w = lhs.w + rhs.w;
    return result;
}

constexpr Vector4 operator-(Vector4 const& lhs, Vector4 const& rhs) noexcept
{
    Vector4 result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    result.z = lhs.z - rhs.z;
    result.w = lhs.w - rhs.w;
    return result;
}

constexpr Vector4 operator*(Vector4 const& vec, Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Vector4 result;
    result.x = vec.x * fVal;
    result.y = vec.y * fVal;
    result.z = vec.z * fVal;
    result.w = vec.w * fVal;
    return result;
}

constexpr Vector4 operator/(Vector4 const& vec, Float32ConvertibleT auto val) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Vector4 result;
    result.x = vec.x / fVal;
    result.y = vec.y / fVal;
    result.z = vec.z / fVal;
    result.w = vec.w / fVal;
    return result;
}

constexpr Vector4 operator*(Float32ConvertibleT auto val, Vector4 const& vec) noexcept
{
    const float32 fVal = static_cast<float32>(val);
    Vector4 result;
    result.x = vec.x * fVal;
    result.y = vec.y * fVal;
    result.z = vec.z * fVal;
    result.w = vec.w * fVal;
    return result;
}

} // namespace mini