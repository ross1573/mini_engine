export module mini.math:vector2_int;

import mini.type;
import :type;
import :operation;
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
    constexpr Vector2Int(int32, int32) noexcept;
    constexpr Vector2Int(uint32, uint32) noexcept;
    explicit constexpr Vector2Int(Vector2 const&) noexcept;
    explicit constexpr Vector2Int(Int32ConvertibleT auto) noexcept;

    /*constexpr*/ float32 Length() const noexcept;
    constexpr SizeT LengthSquared() const noexcept;

    constexpr void Clamp(Vector2Int const&, Vector2Int const&) noexcept;

    static /*constexpr*/ float32 Distance(Vector2Int const&, Vector2Int const&) noexcept;
    static constexpr SizeT DistanceSquared(Vector2Int const&, Vector2Int const&) noexcept;

    constexpr Vector2Int& operator+=(Vector2Int const&) noexcept;
    constexpr Vector2Int& operator-=(Vector2Int const&) noexcept;
    constexpr Vector2Int& operator*=(Int32ConvertibleT auto) noexcept;
    constexpr Vector2Int& operator/=(Int32ConvertibleT auto) noexcept;

    constexpr Vector2Int operator+() const noexcept;
    constexpr Vector2Int operator-() const noexcept;

    constexpr bool operator==(Vector2Int const&) const noexcept;

    explicit constexpr operator Vector2() const noexcept;

    CORE_API static const Vector2Int zero;
    CORE_API static const Vector2Int one;
    CORE_API static const Vector2Int unitX;
    CORE_API static const Vector2Int unitY;
};

export constexpr Vector2Int operator+(Vector2Int const&, Vector2Int const&) noexcept;
export constexpr Vector2Int operator-(Vector2Int const&, Vector2Int const&) noexcept;
export constexpr Vector2Int operator*(Vector2Int const&, Int32ConvertibleT auto const) noexcept;
export constexpr Vector2Int operator/(Vector2Int const&, Int32ConvertibleT auto const) noexcept;
export constexpr Vector2Int operator*(Int32ConvertibleT auto const, Vector2Int const&) noexcept;

inline Vector2Int const Vector2Int::zero = Vector2Int();
inline Vector2Int const Vector2Int::one = Vector2Int(1, 1);
inline Vector2Int const Vector2Int::unitX = Vector2Int(1, 0);
inline Vector2Int const Vector2Int::unitY = Vector2Int(0, 1);

inline constexpr Vector2Int::Vector2Int() noexcept
    : x(0)
    , y(0)
{
}

inline constexpr Vector2Int::Vector2Int(int32 inX, int32 inY) noexcept
    : x(inX)
    , y(inY)
{
}

inline constexpr Vector2Int::Vector2Int(uint32 inX, uint32 inY) noexcept
    : x(static_cast<int32>(inX))
    , y(static_cast<int32>(inY))
{
}

inline constexpr Vector2Int::Vector2Int(Vector2 const& v) noexcept
    : x(static_cast<int32>(v.x))
    , y(static_cast<int32>(v.y))
{
}

inline constexpr Vector2Int::Vector2Int(Int32ConvertibleT auto val) noexcept
    : x(static_cast<int32>(val))
    , y(static_cast<int32>(val))
{
}

inline /*constexpr*/ float32 Vector2Int::Length() const noexcept
{
    return Sqrt(static_cast<SizeT>(PowInt(x, 2u) + PowInt(y, 2u)));
}

inline constexpr SizeT Vector2Int::LengthSquared() const noexcept
{
    return static_cast<SizeT>(PowInt(x, 2u) + PowInt(y, 2u));
}

inline constexpr void Vector2Int::Clamp(Vector2Int const& v1, Vector2Int const& v2) noexcept
{
    x = (x < v1.x) ? v1.x : ((x > v2.x) ? v2.x : x);
    y = (y < v1.y) ? v1.y : ((y > v2.y) ? v2.y : y);
}

inline /*constexpr*/ float32 Vector2Int::Distance(Vector2Int const& v1,
                                                  Vector2Int const& v2) noexcept
{
    return (v1 - v2).Length();
}

inline constexpr SizeT Vector2Int::DistanceSquared(Vector2Int const& v1,
                                                   Vector2Int const& v2) noexcept
{
    return static_cast<SizeT>((v1 - v2).LengthSquared());
}

inline constexpr Vector2Int& Vector2Int::operator+=(Vector2Int const& v) noexcept
{
    x += v.x;
    y += v.y;
    return *this;
}

inline constexpr Vector2Int& Vector2Int::operator-=(Vector2Int const& v) noexcept
{
    x -= v.x;
    y -= v.y;
    return *this;
}

inline constexpr Vector2Int& Vector2Int::operator*=(Int32ConvertibleT auto val) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    x *= iVal;
    y *= iVal;
    return *this;
}

inline constexpr Vector2Int& Vector2Int::operator/=(Int32ConvertibleT auto val) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    x /= iVal;
    y /= iVal;
    return *this;
}

inline constexpr Vector2Int Vector2Int::operator+() const noexcept
{
    return Vector2Int(x, y);
}

inline constexpr Vector2Int Vector2Int::operator-() const noexcept
{
    return Vector2Int(-x, -y);
}

inline constexpr bool Vector2Int::operator==(Vector2Int const& v) const noexcept
{
    return x == v.x && y == v.y;
}

inline constexpr Vector2Int::operator Vector2() const noexcept
{
    Vector2 result;
    result.x = static_cast<float32>(x);
    result.y = static_cast<float32>(y);
    return result;
}

inline constexpr Vector2Int operator+(Vector2Int const& v1, Vector2Int const& v2) noexcept
{
    Vector2Int result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    return result;
}

inline constexpr Vector2Int operator-(Vector2Int const& v1, Vector2Int const& v2) noexcept
{
    Vector2Int result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    return result;
}

inline constexpr Vector2Int operator*(Vector2Int const& v, Int32ConvertibleT auto val) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    Vector2Int result;
    result.x = v.x * iVal;
    result.y = v.y * iVal;
    return result;
}

inline constexpr Vector2Int operator/(Vector2Int const& v, Int32ConvertibleT auto val) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    Vector2Int result;
    result.x = v.x / iVal;
    result.y = v.y / iVal;
    return result;
}

inline constexpr Vector2Int operator*(Int32ConvertibleT auto val, Vector2Int const& v) noexcept
{
    const int32 iVal = static_cast<int32>(val);
    Vector2Int result;
    result.x = v.x * iVal;
    result.y = v.y * iVal;
    return result;
}

} // namespace mini