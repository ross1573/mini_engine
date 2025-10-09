export module mini.core:rect_int;

import :type;
import :math;
import :vector2;
import :vector2_int;
import :vector3_int;

namespace mini {

export struct RectInt {
public:
    int32 x;
    int32 y;
    int32 width;
    int32 height;

    constexpr RectInt() noexcept;
    constexpr RectInt(int32, int32, int32, int32) noexcept;
    constexpr RectInt(Vector2Int const&, Vector2Int const&) noexcept;

    constexpr Vector2 Center() const noexcept;
    constexpr Vector2Int Position() const noexcept;
    constexpr Vector2Int Size() const noexcept;
    constexpr Vector2Int Max() const noexcept;
    constexpr Vector2Int Min() const noexcept;

    constexpr bool Contains(Vector2Int const&) const noexcept;
    constexpr bool Contains(Vector3Int const&) const noexcept;
    constexpr bool Intersects(RectInt const&) const noexcept;
    constexpr bool Overlaps(RectInt const&) const noexcept;

    static constexpr bool Intersect(RectInt const&, RectInt const&) noexcept;
    static constexpr bool Overlap(RectInt const&, RectInt const&) noexcept;

    constexpr bool operator==(RectInt const&) const noexcept;

    CORE_API static const RectInt zero;
};

inline RectInt const RectInt::zero = RectInt();

inline constexpr RectInt::RectInt() noexcept
    : x(0)
    , y(0)
    , width(0)
    , height(0)
{
}

inline constexpr RectInt::RectInt(int32 inX, int32 inY, int32 inWidth, int32 inHeight) noexcept
    : x(inX)
    , y(inY)
    , width(inWidth)
    , height(inHeight)
{
}

inline constexpr RectInt::RectInt(Vector2Int const& position, Vector2Int const& size) noexcept
    : x(position.x)
    , y(position.y)
    , width(size.x)
    , height(size.y)
{
}

inline constexpr Vector2 RectInt::Center() const noexcept
{
    return Vector2(static_cast<float32>(x) + (static_cast<float32>(width) / 2.f),
                   static_cast<float32>(y) + (static_cast<float32>(height) / 2.f));
}

inline constexpr Vector2Int RectInt::Position() const noexcept
{
    return Vector2Int(x, y);
}

inline constexpr Vector2Int RectInt::Size() const noexcept
{
    return Vector2Int(width, height);
}

inline constexpr Vector2Int RectInt::Max() const noexcept
{
    return Vector2Int(x + width, y + height);
}

inline constexpr Vector2Int RectInt::Min() const noexcept
{
    return Vector2Int(x, y);
}

inline constexpr bool RectInt::Contains(Vector2Int const& v) const noexcept
{
    return (v.x >= x) && (v.x <= (x + width)) && (v.y >= y) && (v.y <= (y + height));
}

inline constexpr bool RectInt::Contains(Vector3Int const& v) const noexcept
{
    return Contains(Vector2Int(v));
}

inline constexpr bool RectInt::Intersects(RectInt const& r) const noexcept
{
    return ((r.x < (x + width)) && (x < (r.x + r.width))) &&
           ((r.y < (y + height)) && (y < (r.y + r.height)));
}

inline constexpr bool RectInt::Overlaps(RectInt const& r) const noexcept
{
    return (r.x > x) && (r.y > y) && (r.width < width) && (r.height < height);
}

inline constexpr bool RectInt::Intersect(RectInt const& r1, RectInt const& r2) noexcept
{
    return r1.Intersects(r2);
}

inline constexpr bool RectInt::Overlap(RectInt const& r1, RectInt const& r2) noexcept
{
    return r1.Overlaps(r2);
}

inline constexpr bool RectInt::operator==(RectInt const& r) const noexcept
{
    return x == r.x && y == r.y && width == r.width && height == r.height;
}

} // namespace mini