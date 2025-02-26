module;

#include "mini_engine.h"
#include "core/define.h"
#include "core/type.h"

export module mini.math:vector3_int;

import :common;
import :vector2;
import :vector2_int;
import :vector3;

export namespace mini
{

struct Vector3Int
{
    union
    {
        struct
        {
            int32 x;
            int32 y;
            int32 z;
        };

        int32 data[3];
    };

    constexpr Vector3Int() noexcept;
    constexpr Vector3Int(int32, int32, int32) noexcept;
    constexpr Vector3Int(uint32, uint32, uint32) noexcept;
    explicit constexpr Vector3Int(Vector3 const&) noexcept;
    explicit constexpr Vector3Int(Int32ConvertibleT auto) noexcept;

    /*constexpr*/ float32 Length() const noexcept;
    /*constexpr*/ SizeT LengthSquared() const noexcept;

    constexpr void Clamp(Vector3Int const&, Vector3Int const&) noexcept;

    static /*constexpr*/ float32 Distance(Vector3Int const&, Vector3Int const&) noexcept;
    static /*constexpr*/ SizeT DistanceSquared(Vector3Int const&, Vector3Int const&) noexcept;

    constexpr Vector3Int& operator+=(Vector3Int const&) noexcept;
    constexpr Vector3Int& operator-=(Vector3Int const&) noexcept;
    constexpr Vector3Int& operator*=(Int32ConvertibleT auto) noexcept;
    constexpr Vector3Int& operator/=(Int32ConvertibleT auto) noexcept;

    constexpr Vector3Int operator+() const noexcept;
    constexpr Vector3Int operator-() const noexcept;

    constexpr bool operator==(Vector3Int const&) const noexcept;

    constexpr explicit operator Vector2Int() const noexcept;
    constexpr explicit operator Vector2() const noexcept;
    constexpr explicit operator Vector3() const noexcept;

    static const Vector3Int zero;
    static const Vector3Int one;
    static const Vector3Int unitX;
    static const Vector3Int unitY;
    static const Vector3Int unitZ;
    static const Vector3Int up;
    static const Vector3Int down;
    static const Vector3Int left;
    static const Vector3Int right;
    static const Vector3Int forward;
    static const Vector3Int backward;
};

constexpr Vector3Int operator+(Vector3Int const&, Vector3Int const&) noexcept;
constexpr Vector3Int operator-(Vector3Int const&, Vector3Int const&) noexcept;
constexpr Vector3Int operator*(Vector3Int const&, Int32ConvertibleT auto const) noexcept;
constexpr Vector3Int operator/(Vector3Int const&, Int32ConvertibleT auto const) noexcept;
constexpr Vector3Int operator*(Int32ConvertibleT auto const, Vector3Int const&) noexcept;

inline Vector3Int const Vector3Int::zero = Vector3Int();
inline Vector3Int const Vector3Int::one = Vector3Int(1, 1, 1);
inline Vector3Int const Vector3Int::unitX = Vector3Int(1, 0, 0);
inline Vector3Int const Vector3Int::unitY = Vector3Int(0, 1, 0);
inline Vector3Int const Vector3Int::unitZ = Vector3Int(0, 0, 1);
inline Vector3Int const Vector3Int::up = Vector3Int(0, 1, 0);
inline Vector3Int const Vector3Int::down = Vector3Int(0, -1, 0);
inline Vector3Int const Vector3Int::left = Vector3Int(-1, 0, 0);
inline Vector3Int const Vector3Int::right = Vector3Int(1, 0, 0);
inline Vector3Int const Vector3Int::forward = Vector3Int(0, 0, 1);
inline Vector3Int const Vector3Int::backward = Vector3Int(0, 0, -1);

constexpr Vector3Int::Vector3Int() noexcept
    : x(0), y(0), z(0)
{
}

constexpr Vector3Int::Vector3Int(int32 inX, int32 inY, int32 inZ) noexcept
    : x(inX), y(inY), z(inZ)
{
}

constexpr Vector3Int::Vector3Int(uint32 inX, uint32 inY, uint32 inZ) noexcept
    : x(static_cast<int32>(inX))
    , y(static_cast<int32>(inY))
    , z(static_cast<int32>(inZ))
{
}

constexpr Vector3Int::Vector3Int(Vector3 const& v) noexcept
    : x(static_cast<int32>(v.x))
    , y(static_cast<int32>(v.y))
    , z(static_cast<int32>(v.z))
{
}

constexpr Vector3Int::Vector3Int(Int32ConvertibleT auto val) noexcept
    : x(static_cast<int32>(val))
    , y(static_cast<int32>(val))
    , z(static_cast<int32>(val))
{
}

/*constexpr*/ inline float32 Vector3Int::Length() const noexcept
{
    float32 xPow = Pow(static_cast<float32>(x), 2.f);
    float32 yPow = Pow(static_cast<float32>(y), 2.f);
    float32 zPow = Pow(static_cast<float32>(z), 2.f);
    return Sqrt(xPow + yPow + zPow);
}

/*constexpr*/ inline SizeT Vector3Int::LengthSquared() const noexcept
{
    return static_cast<SizeT>(Pow(x, 2) + Pow(y, 2) + Pow(z, 2));
}

constexpr void Vector3Int::Clamp(Vector3Int const& v1, Vector3Int const& v2) noexcept
{
    x = (x < v1.x) ? v1.x : ((x > v2.x) ? v2.x : x);
    y = (y < v1.y) ? v1.y : ((y > v2.y) ? v2.y : y);
    z = (z < v1.z) ? v1.z : ((z > v2.z) ? v2.z : z);
}

/*constexpr*/ inline float32 Vector3Int::Distance(Vector3Int const& v1,
                                                  Vector3Int const& v2) noexcept
{
    return (v1 - v2).Length();
}

/*constexpr*/ inline SizeT Vector3Int::DistanceSquared(Vector3Int const& v1,
                                                       Vector3Int const& v2) noexcept
{
    return static_cast<SizeT>((v1 - v2).LengthSquared());
}

constexpr Vector3Int& Vector3Int::operator+=(Vector3Int const& v) noexcept
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

constexpr Vector3Int& Vector3Int::operator-=(Vector3Int const& v) noexcept
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

constexpr Vector3Int& Vector3Int::operator*=(Int32ConvertibleT auto val) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    x *= iVal;
    y *= iVal;
    z *= iVal;
    return *this;
}

constexpr Vector3Int& Vector3Int::operator/=(Int32ConvertibleT auto val) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    x /= iVal;
    y /= iVal;
    z /= iVal;
    return *this;
}

constexpr Vector3Int Vector3Int::operator+() const noexcept
{
    return Vector3Int(x, y, z);
}

constexpr Vector3Int Vector3Int::operator-() const noexcept
{
    return Vector3Int(-x, -y, -z);
}

constexpr bool Vector3Int::operator==(Vector3Int const& v) const noexcept
{
    return x == v.x && y == v.y && z == v.z;
}

constexpr Vector3Int::operator Vector2Int() const noexcept
{
    return Vector2Int(x, y);
}

constexpr Vector3Int::operator Vector2() const noexcept
{
    Vector2 result;
    result.x = static_cast<float32>(x);
    result.y = static_cast<float32>(y);
    return result;
}

constexpr Vector3Int::operator Vector3() const noexcept
{
    Vector3 result;
    result.x = static_cast<float32>(x);
    result.y = static_cast<float32>(y);
    result.z = static_cast<float32>(z);
    return result;
}

constexpr Vector3Int operator+(Vector3Int const& v1, Vector3Int const& v2) noexcept
{
    Vector3Int result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    result.z = v1.z + v2.z;
    return result;
}

constexpr Vector3Int operator-(Vector3Int const& v1, Vector3Int const& v2) noexcept
{
    Vector3Int result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
    return result;
}

constexpr Vector3Int operator*(Vector3Int const& v, Int32ConvertibleT auto val) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    Vector3Int result;
    result.x = v.x * iVal;
    result.y = v.y * iVal;
    result.z = v.z * iVal;
    return result;
}

constexpr Vector3Int operator/(Vector3Int const& v, Int32ConvertibleT auto val) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    Vector3Int result;
    result.x = v.x / iVal;
    result.y = v.y / iVal;
    result.z = v.z / iVal;
    return result;
}

constexpr Vector3Int operator*(Int32ConvertibleT auto val, Vector3Int const& v) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    Vector3Int result;
    result.x = v.x * iVal;
    result.y = v.y * iVal;
    result.z = v.z * iVal;
    return result;
}

} // namespace mini