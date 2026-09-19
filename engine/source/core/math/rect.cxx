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

    [[nodiscard]] constexpr Vector2 Center() const noexcept;
    [[nodiscard]] constexpr Vector2 Position() const noexcept;
    [[nodiscard]] constexpr Vector2 Size() const noexcept;
    [[nodiscard]] constexpr Vector2 Max() const noexcept;
    [[nodiscard]] constexpr Vector2 Min() const noexcept;

    [[nodiscard]] constexpr bool Contains(Vector2 const& position) const noexcept;
    [[nodiscard]] constexpr bool Contains(Vector3 const& position) const noexcept;
    [[nodiscard]] constexpr bool Intersects(Rect const& rect) const noexcept;
    [[nodiscard]] constexpr bool Overlaps(Rect const& rect) const noexcept;

    static constexpr bool Intersect(Rect const& lhs, Rect const& rhs) noexcept;
    static constexpr bool Overlap(Rect const& lhs, Rect const& rhs) noexcept;

    constexpr bool operator==(Rect const& other) const noexcept;

    static constexpr Rect Zero() noexcept { return { 0.f, 0.f, 0.f, 0.f }; }
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

constexpr Vector2 Rect::Center() const noexcept
{
    return { x + (width / 2), y + (height / 2) };
}

constexpr Vector2 Rect::Position() const noexcept
{
    return { x, y };
}

constexpr Vector2 Rect::Size() const noexcept
{
    return { width, height };
}

constexpr Vector2 Rect::Max() const noexcept
{
    return { x + width, y + height };
}

constexpr Vector2 Rect::Min() const noexcept
{
    return { x, y };
}

constexpr bool Rect::Contains(Vector2 const& position) const noexcept
{
    return (position.x >= x) && (position.x <= (x + width)) && (position.y >= y) && (position.y <= (y + height));
}

constexpr bool Rect::Contains(Vector3 const& position) const noexcept
{
    return Contains(Vector2(position));
}

constexpr bool Rect::Intersects(Rect const& rect) const noexcept
{
    return ((rect.x < (x + width)) && (x < (rect.x + rect.width))) &&
           ((rect.y < (y + height)) && (y < (rect.y + rect.height)));
}

constexpr bool Rect::Overlaps(Rect const& rect) const noexcept
{
    return (rect.x > x) && (rect.y > y) && (rect.width < width) && (rect.height < height);
}

constexpr bool Rect::Intersect(Rect const& lhs, Rect const& rhs) noexcept
{
    return lhs.Intersects(rhs);
}

constexpr bool Rect::Overlap(Rect const& lhs, Rect const& rhs) noexcept
{
    return lhs.Overlaps(rhs);
}

constexpr bool Rect::operator==(Rect const& other) const noexcept
{
    return x == other.x && y == other.y && width == other.width && height == other.height;
}

} // namespace mini