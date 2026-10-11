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

    [[nodiscard]] constexpr Vector2 center() const noexcept;
    [[nodiscard]] constexpr Vector2Int position() const noexcept;
    [[nodiscard]] constexpr Vector2Int size() const noexcept;
    [[nodiscard]] constexpr Vector2Int max() const noexcept;
    [[nodiscard]] constexpr Vector2Int min() const noexcept;

    [[nodiscard]] constexpr bool contains(Vector2Int const& position) const noexcept;
    [[nodiscard]] constexpr bool contains(Vector3Int const& position) const noexcept;
    [[nodiscard]] constexpr bool intersects(RectInt const& rect) const noexcept;
    [[nodiscard]] constexpr bool overlaps(RectInt const& rect) const noexcept;

    static constexpr bool intersect(RectInt const& lhs, RectInt const& rhs) noexcept;
    static constexpr bool overlap(RectInt const& lhs, RectInt const& rhs) noexcept;

    constexpr bool operator==(RectInt const& other) const noexcept;

    static constexpr RectInt zero() noexcept { return {0, 0, 0, 0}; }
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

constexpr Vector2 RectInt::center() const noexcept
{
    return {static_cast<float32>(x) + (static_cast<float32>(width) / 2.f),
            static_cast<float32>(y) + (static_cast<float32>(height) / 2.f)};
}

constexpr Vector2Int RectInt::position() const noexcept
{
    return {x, y};
}

constexpr Vector2Int RectInt::size() const noexcept
{
    return {width, height};
}

constexpr Vector2Int RectInt::max() const noexcept
{
    return {x + width, y + height};
}

constexpr Vector2Int RectInt::min() const noexcept
{
    return {x, y};
}

constexpr bool RectInt::contains(Vector2Int const& position) const noexcept
{
    return (position.x >= x) && (position.x <= (x + width)) && (position.y >= y) && (position.y <= (y + height));
}

constexpr bool RectInt::contains(Vector3Int const& position) const noexcept
{
    return contains(Vector2Int(position));
}

constexpr bool RectInt::intersects(RectInt const& rect) const noexcept
{
    return ((rect.x < (x + width)) && (x < (rect.x + rect.width))) &&
           ((rect.y < (y + height)) && (y < (rect.y + rect.height)));
}

constexpr bool RectInt::overlaps(RectInt const& rect) const noexcept
{
    return (rect.x > x) && (rect.y > y) && (rect.width < width) && (rect.height < height);
}

constexpr bool RectInt::intersect(RectInt const& lhs, RectInt const& rhs) noexcept
{
    return lhs.intersects(rhs);
}

constexpr bool RectInt::overlap(RectInt const& lhs, RectInt const& rhs) noexcept
{
    return lhs.overlaps(rhs);
}

constexpr bool RectInt::operator==(RectInt const& other) const noexcept
{
    return x == other.x && y == other.y && width == other.width && height == other.height;
}

} // namespace mini