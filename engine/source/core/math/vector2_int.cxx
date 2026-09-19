export module mini.core:vector2_int;

import :type;
import :math_type;
import :math_operation;
import :vector2;

namespace mini {

export struct Vector2Int {
public:
    union {
        struct {
            int32 x;
            int32 y;
        };

        int32 data[2];
    };

    constexpr Vector2Int() noexcept;
    constexpr Vector2Int(int32 x, int32 y) noexcept;
    constexpr Vector2Int(uint32 x, uint32 y) noexcept;
    explicit constexpr Vector2Int(Vector2 const& vec) noexcept;
    explicit constexpr Vector2Int(Int32ConvertibleT auto val) noexcept;

    [[nodiscard]] /*constexpr*/ float32 Length() const noexcept;
    [[nodiscard]] constexpr size_t LengthSquared() const noexcept;

    constexpr void Clamp(Vector2Int const& min, Vector2Int const& max) noexcept;

    static /*constexpr*/ float32 Distance(Vector2Int const& lhs, Vector2Int const& rhs) noexcept;
    static constexpr size_t DistanceSquared(Vector2Int const& lhs, Vector2Int const& rhs) noexcept;

    constexpr Vector2Int& operator+=(Vector2Int const& ivec) noexcept;
    constexpr Vector2Int& operator-=(Vector2Int const& ivec) noexcept;
    constexpr Vector2Int& operator*=(Int32ConvertibleT auto val) noexcept;
    constexpr Vector2Int& operator/=(Int32ConvertibleT auto val) noexcept;

    constexpr Vector2Int operator+() const noexcept;
    constexpr Vector2Int operator-() const noexcept;

    constexpr bool operator==(Vector2Int const& vec) const noexcept;

    explicit constexpr operator Vector2() const noexcept;

    static constexpr Vector2Int Zero() noexcept { return { 0, 0 }; }
    static constexpr Vector2Int One() noexcept { return { 1, 1 }; }
    static constexpr Vector2Int UnitX() noexcept { return { 1, 0 }; }
    static constexpr Vector2Int UnitY() noexcept { return { 0, 1 }; }
};

export constexpr Vector2Int operator+(Vector2Int const& lhs, Vector2Int const& rhs) noexcept;
export constexpr Vector2Int operator-(Vector2Int const& lhs, Vector2Int const& rhs) noexcept;
export constexpr Vector2Int operator*(Vector2Int const& vec, Int32ConvertibleT auto val) noexcept;
export constexpr Vector2Int operator/(Vector2Int const& vec, Int32ConvertibleT auto val) noexcept;
export constexpr Vector2Int operator*(Int32ConvertibleT auto val, Vector2Int const& vec) noexcept;

constexpr Vector2Int::Vector2Int() noexcept
    : x(0)
    , y(0)
{
}

constexpr Vector2Int::Vector2Int(int32 x, int32 y) noexcept
    : x(x)
    , y(y)
{
}

constexpr Vector2Int::Vector2Int(uint32 x, uint32 y) noexcept
    : x(static_cast<int32>(x))
    , y(static_cast<int32>(y))
{
}

constexpr Vector2Int::Vector2Int(Vector2 const& vec) noexcept
    : x(static_cast<int32>(vec.x))
    , y(static_cast<int32>(vec.y))
{
}

constexpr Vector2Int::Vector2Int(Int32ConvertibleT auto val) noexcept
    : x(static_cast<int32>(val))
    , y(static_cast<int32>(val))
{
}

inline /*constexpr*/ float32 Vector2Int::Length() const noexcept
{
    return Sqrt(static_cast<size_t>(PowInt(x, 2u) + PowInt(y, 2u)));
}

constexpr size_t Vector2Int::LengthSquared() const noexcept
{
    return static_cast<size_t>(PowInt(x, 2u) + PowInt(y, 2u));
}

constexpr void Vector2Int::Clamp(Vector2Int const& min, Vector2Int const& max) noexcept
{
    x = (x < min.x) ? min.x : ((x > max.x) ? max.x : x);
    y = (y < min.y) ? min.y : ((y > max.y) ? max.y : y);
}

inline /*constexpr*/ float32 Vector2Int::Distance(Vector2Int const& lhs, Vector2Int const& rhs) noexcept
{
    return (lhs - rhs).Length();
}

constexpr size_t Vector2Int::DistanceSquared(Vector2Int const& lhs, Vector2Int const& rhs) noexcept
{
    return (lhs - rhs).LengthSquared();
}

constexpr Vector2Int& Vector2Int::operator+=(Vector2Int const& vec) noexcept
{
    x += vec.x;
    y += vec.y;
    return *this;
}

constexpr Vector2Int& Vector2Int::operator-=(Vector2Int const& vec) noexcept
{
    x -= vec.x;
    y -= vec.y;
    return *this;
}

constexpr Vector2Int& Vector2Int::operator*=(Int32ConvertibleT auto val) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    x *= iVal;
    y *= iVal;
    return *this;
}

constexpr Vector2Int& Vector2Int::operator/=(Int32ConvertibleT auto val) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    x /= iVal;
    y /= iVal;
    return *this;
}

constexpr Vector2Int Vector2Int::operator+() const noexcept
{
    return { x, y };
}

constexpr Vector2Int Vector2Int::operator-() const noexcept
{
    return { -x, -y };
}

constexpr bool Vector2Int::operator==(Vector2Int const& vec) const noexcept
{
    return x == vec.x && y == vec.y;
}

constexpr Vector2Int::operator Vector2() const noexcept
{
    Vector2 result;
    result.x = static_cast<float32>(x);
    result.y = static_cast<float32>(y);
    return result;
}

constexpr Vector2Int operator+(Vector2Int const& lhs, Vector2Int const& rhs) noexcept
{
    Vector2Int result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    return result;
}

constexpr Vector2Int operator-(Vector2Int const& lhs, Vector2Int const& rhs) noexcept
{
    Vector2Int result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    return result;
}

constexpr Vector2Int operator*(Vector2Int const& vec, Int32ConvertibleT auto val) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    Vector2Int result;
    result.x = vec.x * iVal;
    result.y = vec.y * iVal;
    return result;
}

constexpr Vector2Int operator/(Vector2Int const& vec, Int32ConvertibleT auto val) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    Vector2Int result;
    result.x = vec.x / iVal;
    result.y = vec.y / iVal;
    return result;
}

constexpr Vector2Int operator*(Int32ConvertibleT auto val, Vector2Int const& vec) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    Vector2Int result;
    result.x = vec.x * iVal;
    result.y = vec.y * iVal;
    return result;
}

} // namespace mini