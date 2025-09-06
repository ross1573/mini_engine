export module mini.core:vector3_int;

import :type;
import :math;
import :vector2;
import :vector2_int;
import :vector3;

namespace mini {

export struct Vector3Int {
public:
    union {
        struct {
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

    CORE_API static const Vector3Int zero;
    CORE_API static const Vector3Int one;
    CORE_API static const Vector3Int unitX;
    CORE_API static const Vector3Int unitY;
    CORE_API static const Vector3Int unitZ;
    CORE_API static const Vector3Int up;
    CORE_API static const Vector3Int down;
    CORE_API static const Vector3Int left;
    CORE_API static const Vector3Int right;
    CORE_API static const Vector3Int forward;
    CORE_API static const Vector3Int backward;
};

export constexpr Vector3Int operator+(Vector3Int const&, Vector3Int const&) noexcept;
export constexpr Vector3Int operator-(Vector3Int const&, Vector3Int const&) noexcept;
export constexpr Vector3Int operator*(Vector3Int const&, Int32ConvertibleT auto const) noexcept;
export constexpr Vector3Int operator/(Vector3Int const&, Int32ConvertibleT auto const) noexcept;
export constexpr Vector3Int operator*(Int32ConvertibleT auto const, Vector3Int const&) noexcept;

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

inline constexpr Vector3Int::Vector3Int() noexcept
    : x(0)
    , y(0)
    , z(0)
{
}

inline constexpr Vector3Int::Vector3Int(int32 inX, int32 inY, int32 inZ) noexcept
    : x(inX)
    , y(inY)
    , z(inZ)
{
}

inline constexpr Vector3Int::Vector3Int(uint32 inX, uint32 inY, uint32 inZ) noexcept
    : x(static_cast<int32>(inX))
    , y(static_cast<int32>(inY))
    , z(static_cast<int32>(inZ))
{
}

inline constexpr Vector3Int::Vector3Int(Vector3 const& v) noexcept
    : x(static_cast<int32>(v.x))
    , y(static_cast<int32>(v.y))
    , z(static_cast<int32>(v.z))
{
}

inline constexpr Vector3Int::Vector3Int(Int32ConvertibleT auto val) noexcept
    : x(static_cast<int32>(val))
    , y(static_cast<int32>(val))
    , z(static_cast<int32>(val))
{
}

inline /*constexpr*/ float32 Vector3Int::Length() const noexcept
{
    float32 xPow = Pow(static_cast<float32>(x), 2.f);
    float32 yPow = Pow(static_cast<float32>(y), 2.f);
    float32 zPow = Pow(static_cast<float32>(z), 2.f);
    return Sqrt(xPow + yPow + zPow);
}

inline /*constexpr*/ SizeT Vector3Int::LengthSquared() const noexcept
{
    return static_cast<SizeT>(Pow(x, 2) + Pow(y, 2) + Pow(z, 2));
}

inline constexpr void Vector3Int::Clamp(Vector3Int const& v1, Vector3Int const& v2) noexcept
{
    x = (x < v1.x) ? v1.x : ((x > v2.x) ? v2.x : x);
    y = (y < v1.y) ? v1.y : ((y > v2.y) ? v2.y : y);
    z = (z < v1.z) ? v1.z : ((z > v2.z) ? v2.z : z);
}

inline /*constexpr*/ float32 Vector3Int::Distance(Vector3Int const& v1,
                                                  Vector3Int const& v2) noexcept
{
    return (v1 - v2).Length();
}

inline /*constexpr*/ SizeT Vector3Int::DistanceSquared(Vector3Int const& v1,
                                                       Vector3Int const& v2) noexcept
{
    return static_cast<SizeT>((v1 - v2).LengthSquared());
}

inline constexpr Vector3Int& Vector3Int::operator+=(Vector3Int const& v) noexcept
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

inline constexpr Vector3Int& Vector3Int::operator-=(Vector3Int const& v) noexcept
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

inline constexpr Vector3Int& Vector3Int::operator*=(Int32ConvertibleT auto val) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    x *= iVal;
    y *= iVal;
    z *= iVal;
    return *this;
}

inline constexpr Vector3Int& Vector3Int::operator/=(Int32ConvertibleT auto val) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    x /= iVal;
    y /= iVal;
    z /= iVal;
    return *this;
}

inline constexpr Vector3Int Vector3Int::operator+() const noexcept
{
    return Vector3Int(x, y, z);
}

inline constexpr Vector3Int Vector3Int::operator-() const noexcept
{
    return Vector3Int(-x, -y, -z);
}

inline constexpr bool Vector3Int::operator==(Vector3Int const& v) const noexcept
{
    return x == v.x && y == v.y && z == v.z;
}

inline constexpr Vector3Int::operator Vector2Int() const noexcept
{
    return Vector2Int(x, y);
}

inline constexpr Vector3Int::operator Vector2() const noexcept
{
    Vector2 result;
    result.x = static_cast<float32>(x);
    result.y = static_cast<float32>(y);
    return result;
}

inline constexpr Vector3Int::operator Vector3() const noexcept
{
    Vector3 result;
    result.x = static_cast<float32>(x);
    result.y = static_cast<float32>(y);
    result.z = static_cast<float32>(z);
    return result;
}

inline constexpr Vector3Int operator+(Vector3Int const& v1, Vector3Int const& v2) noexcept
{
    Vector3Int result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    result.z = v1.z + v2.z;
    return result;
}

inline constexpr Vector3Int operator-(Vector3Int const& v1, Vector3Int const& v2) noexcept
{
    Vector3Int result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
    return result;
}

inline constexpr Vector3Int operator*(Vector3Int const& v, Int32ConvertibleT auto val) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    Vector3Int result;
    result.x = v.x * iVal;
    result.y = v.y * iVal;
    result.z = v.z * iVal;
    return result;
}

inline constexpr Vector3Int operator/(Vector3Int const& v, Int32ConvertibleT auto val) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    Vector3Int result;
    result.x = v.x / iVal;
    result.y = v.y / iVal;
    result.z = v.z / iVal;
    return result;
}

inline constexpr Vector3Int operator*(Int32ConvertibleT auto val, Vector3Int const& v) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    Vector3Int result;
    result.x = v.x * iVal;
    result.y = v.y * iVal;
    result.z = v.z * iVal;
    return result;
}

} // namespace mini