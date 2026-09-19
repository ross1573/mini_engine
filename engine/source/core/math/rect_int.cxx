export module mini.core:rect_int;

import :type;
import :math_type;
import :math_operation;
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
    constexpr RectInt(int32 x, int32 y, int32 width, int32 height) noexcept;
    constexpr RectInt(Vector2Int const& position, Vector2Int const& size) noexcept;

    [[nodiscard]] constexpr Vector2 Center() const noexcept;
    [[nodiscard]] constexpr Vector2Int Position() const noexcept;
    [[nodiscard]] constexpr Vector2Int Size() const noexcept;
    [[nodiscard]] constexpr Vector2Int Max() const noexcept;
    [[nodiscard]] constexpr Vector2Int Min() const noexcept;

    [[nodiscard]] constexpr bool Contains(Vector2Int const& position) const noexcept;
    [[nodiscard]] constexpr bool Contains(Vector3Int const& position) const noexcept;
    [[nodiscard]] constexpr bool Intersects(RectInt const& rect) const noexcept;
    [[nodiscard]] constexpr bool Overlaps(RectInt const& rect) const noexcept;

    static constexpr bool Intersect(RectInt const& lhs, RectInt const& rhs) noexcept;
    static constexpr bool Overlap(RectInt const& lhs, RectInt const& rhs) noexcept;

    constexpr bool operator==(RectInt const& other) const noexcept;

    static constexpr RectInt Zero() noexcept { return { 0, 0, 0, 0 }; }
};

constexpr RectInt::RectInt() noexcept
    : x(0)
    , y(0)
    , width(0)
    , height(0)
{
}

constexpr RectInt::RectInt(int32 x, int32 y, int32 width, int32 height) noexcept
    : x(x)
    , y(y)
    , width(width)
    , height(height)
{
}

constexpr RectInt::RectInt(Vector2Int const& position, Vector2Int const& size) noexcept
    : x(position.x)
    , y(position.y)
    , width(size.x)
    , height(size.y)
{
}

constexpr Vector2 RectInt::Center() const noexcept
{
    return { static_cast<float32>(x) + (static_cast<float32>(width) / 2.f),
             static_cast<float32>(y) + (static_cast<float32>(height) / 2.f) };
}

constexpr Vector2Int RectInt::Position() const noexcept
{
    return { x, y };
}

constexpr Vector2Int RectInt::Size() const noexcept
{
    return { width, height };
}

constexpr Vector2Int RectInt::Max() const noexcept
{
    return { x + width, y + height };
}

constexpr Vector2Int RectInt::Min() const noexcept
{
    return { x, y };
}

constexpr bool RectInt::Contains(Vector2Int const& position) const noexcept
{
    return (position.x >= x) && (position.x <= (x + width)) && (position.y >= y) && (position.y <= (y + height));
}

constexpr bool RectInt::Contains(Vector3Int const& position) const noexcept
{
    return Contains(Vector2Int(position));
}

constexpr bool RectInt::Intersects(RectInt const& rect) const noexcept
{
    return ((rect.x < (x + width)) && (x < (rect.x + rect.width))) &&
           ((rect.y < (y + height)) && (y < (rect.y + rect.height)));
}

constexpr bool RectInt::Overlaps(RectInt const& rect) const noexcept
{
    return (rect.x > x) && (rect.y > y) && (rect.width < width) && (rect.height < height);
}

constexpr bool RectInt::Intersect(RectInt const& lhs, RectInt const& rhs) noexcept
{
    return lhs.Intersects(rhs);
}

constexpr bool RectInt::Overlap(RectInt const& lhs, RectInt const& rhs) noexcept
{
    return lhs.Overlaps(rhs);
}

constexpr bool RectInt::operator==(RectInt const& other) const noexcept
{
    return x == other.x && y == other.y && width == other.width && height == other.height;
}

} // namespace mini