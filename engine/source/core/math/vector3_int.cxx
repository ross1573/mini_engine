export module mini.core:vector3_int;

import :type;
import :math_type;
import :math_operation;
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
    constexpr Vector3Int(int32 x, int32 y, int32 z) noexcept;
    constexpr Vector3Int(uint32 x, uint32 y, uint32 z) noexcept;
    explicit constexpr Vector3Int(Vector3 const& vec) noexcept;
    explicit constexpr Vector3Int(Int32ConvertibleT auto val) noexcept;

    [[nodiscard]] /*constexpr*/ float32 Length() const noexcept;
    [[nodiscard]] constexpr size_t LengthSquared() const noexcept;

    constexpr void Clamp(Vector3Int const& min, Vector3Int const& max) noexcept;

    static /*constexpr*/ float32 Distance(Vector3Int const& lhs, Vector3Int const& rhs) noexcept;
    static constexpr size_t DistanceSquared(Vector3Int const& lhs, Vector3Int const& rhs) noexcept;

    constexpr Vector3Int& operator+=(Vector3Int const& vec) noexcept;
    constexpr Vector3Int& operator-=(Vector3Int const& vec) noexcept;
    constexpr Vector3Int& operator*=(Int32ConvertibleT auto val) noexcept;
    constexpr Vector3Int& operator/=(Int32ConvertibleT auto val) noexcept;

    constexpr Vector3Int operator+() const noexcept;
    constexpr Vector3Int operator-() const noexcept;

    constexpr bool operator==(Vector3Int const& other) const noexcept;

    constexpr explicit operator Vector2Int() const noexcept;
    constexpr explicit operator Vector2() const noexcept;
    constexpr explicit operator Vector3() const noexcept;

    static constexpr Vector3Int Zero() noexcept { return { 0, 0, 0 }; }
    static constexpr Vector3Int One() noexcept { return { 1, 1, 1 }; }
    static constexpr Vector3Int UnitX() noexcept { return { 1, 0, 0 }; }
    static constexpr Vector3Int UnitY() noexcept { return { 0, 1, 0 }; }
    static constexpr Vector3Int UnitZ() noexcept { return { 0, 0, 1 }; }
    static constexpr Vector3Int Up() noexcept { return { 0, 1, 0 }; }
    static constexpr Vector3Int Down() noexcept { return { 0, -1, 0 }; }
    static constexpr Vector3Int Left() noexcept { return { -1, 0, 0 }; }
    static constexpr Vector3Int Right() noexcept { return { 1, 0, 0 }; }
    static constexpr Vector3Int Forward() noexcept { return { 0, 0, 1 }; }
    static constexpr Vector3Int Backward() noexcept { return { 0, 0, -1 }; }
};

export constexpr Vector3Int operator+(Vector3Int const& lhs, Vector3Int const& rhs) noexcept;
export constexpr Vector3Int operator-(Vector3Int const& lhs, Vector3Int const& rhs) noexcept;
export constexpr Vector3Int operator*(Vector3Int const& vec, Int32ConvertibleT auto val) noexcept;
export constexpr Vector3Int operator/(Vector3Int const& vec, Int32ConvertibleT auto val) noexcept;
export constexpr Vector3Int operator*(Int32ConvertibleT auto val, Vector3Int const& vec) noexcept;

constexpr Vector3Int::Vector3Int() noexcept
    : x(0)
    , y(0)
    , z(0)
{
}

constexpr Vector3Int::Vector3Int(int32 x, int32 y, int32 z) noexcept
    : x(x)
    , y(y)
    , z(z)
{
}

constexpr Vector3Int::Vector3Int(uint32 x, uint32 y, uint32 z) noexcept
    : x(static_cast<int32>(x))
    , y(static_cast<int32>(y))
    , z(static_cast<int32>(z))
{
}

constexpr Vector3Int::Vector3Int(Vector3 const& vec) noexcept
    : x(static_cast<int32>(vec.x))
    , y(static_cast<int32>(vec.y))
    , z(static_cast<int32>(vec.z))
{
}

constexpr Vector3Int::Vector3Int(Int32ConvertibleT auto val) noexcept
    : x(static_cast<int32>(val))
    , y(static_cast<int32>(val))
    , z(static_cast<int32>(val))
{
}

inline /*constexpr*/ float32 Vector3Int::Length() const noexcept
{
    return Sqrt(static_cast<size_t>(PowInt(x, 2u) + PowInt(y, 2u) + PowInt(z, 2u)));
}

constexpr size_t Vector3Int::LengthSquared() const noexcept
{
    return static_cast<size_t>(PowInt(x, 2u) + PowInt(y, 2u) + PowInt(z, 2u));
}

constexpr void Vector3Int::Clamp(Vector3Int const& min, Vector3Int const& max) noexcept
{
    x = (x < min.x) ? min.x : ((x > max.x) ? max.x : x);
    y = (y < min.y) ? min.y : ((y > max.y) ? max.y : y);
    z = (z < min.z) ? min.z : ((z > max.z) ? max.z : z);
}

inline /*constexpr*/ float32 Vector3Int::Distance(Vector3Int const& lhs, Vector3Int const& rhs) noexcept
{
    return (lhs - rhs).Length();
}

constexpr size_t Vector3Int::DistanceSquared(Vector3Int const& lhs, Vector3Int const& rhs) noexcept
{
    return (lhs - rhs).LengthSquared();
}

constexpr Vector3Int& Vector3Int::operator+=(Vector3Int const& vec) noexcept
{
    x += vec.x;
    y += vec.y;
    z += vec.z;
    return *this;
}

constexpr Vector3Int& Vector3Int::operator-=(Vector3Int const& vec) noexcept
{
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;
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
    return { x, y, z };
}

constexpr Vector3Int Vector3Int::operator-() const noexcept
{
    return { -x, -y, -z };
}

constexpr bool Vector3Int::operator==(Vector3Int const& other) const noexcept
{
    return x == other.x && y == other.y && z == other.z;
}

constexpr Vector3Int::operator Vector2Int() const noexcept
{
    return { x, y };
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

constexpr Vector3Int operator+(Vector3Int const& lhs, Vector3Int const& rhs) noexcept
{
    Vector3Int result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    result.z = lhs.z + rhs.z;
    return result;
}

constexpr Vector3Int operator-(Vector3Int const& lhs, Vector3Int const& rhs) noexcept
{
    Vector3Int result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    result.z = lhs.z - rhs.z;
    return result;
}

constexpr Vector3Int operator*(Vector3Int const& vec, Int32ConvertibleT auto val) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    Vector3Int result;
    result.x = vec.x * iVal;
    result.y = vec.y * iVal;
    result.z = vec.z * iVal;
    return result;
}

constexpr Vector3Int operator/(Vector3Int const& vec, Int32ConvertibleT auto val) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    Vector3Int result;
    result.x = vec.x / iVal;
    result.y = vec.y / iVal;
    result.z = vec.z / iVal;
    return result;
}

constexpr Vector3Int operator*(Int32ConvertibleT auto val, Vector3Int const& vec) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    Vector3Int result;
    result.x = vec.x * iVal;
    result.y = vec.y * iVal;
    result.z = vec.z * iVal;
    return result;
}

} // namespace mini