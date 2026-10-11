export module mini.core:rect;

import :type;
import :math_type;
import :math_operation;
import :rect_int;
import :vector2;
import :vector3;

namespace mini {

export struct Rect {
public:
    float32 x;
    float32 y;
    float32 width;
    float32 height;

    constexpr Rect() noexcept;
    constexpr Rect(float32 x, float32 y, float32 width, float32 height) noexcept;
    constexpr Rect(Vector2 const& position, Vector2 const& size) noexcept;
    explicit constexpr Rect(RectInt const& rectInt) noexcept;

    [[nodiscard]] constexpr Vector2 center() const noexcept;
    [[nodiscard]] constexpr Vector2 position() const noexcept;
    [[nodiscard]] constexpr Vector2 size() const noexcept;
    [[nodiscard]] constexpr Vector2 max() const noexcept;
    [[nodiscard]] constexpr Vector2 min() const noexcept;

    [[nodiscard]] constexpr bool contains(Vector2 const& position) const noexcept;
    [[nodiscard]] constexpr bool contains(Vector3 const& position) const noexcept;
    [[nodiscard]] constexpr bool intersects(Rect const& rect) const noexcept;
    [[nodiscard]] constexpr bool overlaps(Rect const& rect) const noexcept;

    static constexpr bool intersect(Rect const& lhs, Rect const& rhs) noexcept;
    static constexpr bool overlap(Rect const& lhs, Rect const& rhs) noexcept;

    constexpr bool operator==(Rect const& other) const noexcept;

    static constexpr Rect zero() noexcept { return {0.f, 0.f, 0.f, 0.f}; }
};

constexpr Rect::Rect() noexcept
    : x(0.f)
    , y(0.f)
    , width(0.f)
    , height(0.f)
{
}

constexpr Rect::Rect(float32 x, float32 y, float32 width, float32 height) noexcept
    : x(x)
    , y(y)
    , width(width)
    , height(height)
{
}

constexpr Rect::Rect(Vector2 const& position, Vector2 const& size) noexcept
    : x(position.x)
    , y(position.y)
    , width(size.x)
    , height(size.y)
{
}

constexpr Rect::Rect(RectInt const& rect) noexcept
    : x(static_cast<float32>(rect.x))
    , y(static_cast<float32>(rect.y))
    , width(static_cast<float32>(rect.width))
    , height(static_cast<float32>(rect.height))
{
}

constexpr Vector2 Rect::center() const noexcept
{
    return {x + (width / 2), y + (height / 2)};
}

constexpr Vector2 Rect::position() const noexcept
{
    return {x, y};
}

constexpr Vector2 Rect::size() const noexcept
{
    return {width, height};
}

constexpr Vector2 Rect::max() const noexcept
{
    return {x + width, y + height};
}

constexpr Vector2 Rect::min() const noexcept
{
    return {x, y};
}

constexpr bool Rect::contains(Vector2 const& position) const noexcept
{
    return (position.x >= x) && (position.x <= (x + width)) && (position.y >= y) && (position.y <= (y + height));
}

constexpr bool Rect::contains(Vector3 const& position) const noexcept
{
    return contains(Vector2(position));
}

constexpr bool Rect::intersects(Rect const& rect) const noexcept
{
    return ((rect.x < (x + width)) && (x < (rect.x + rect.width))) &&
           ((rect.y < (y + height)) && (y < (rect.y + rect.height)));
}

constexpr bool Rect::overlaps(Rect const& rect) const noexcept
{
    return (rect.x > x) && (rect.y > y) && (rect.width < width) && (rect.height < height);
}

constexpr bool Rect::intersect(Rect const& lhs, Rect const& rhs) noexcept
{
    return lhs.intersects(rhs);
}

constexpr bool Rect::overlap(Rect const& lhs, Rect const& rhs) noexcept
{
    return lhs.overlaps(rhs);
}

constexpr bool Rect::operator==(Rect const& other) const noexcept
{
    return x == other.x && y == other.y && width == other.width && height == other.height;
}

} // namespace mini