#pragma once

#include "math/common.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/rect_int.h"

namespace mini
{

struct Rect
{
    float32 x;
    float32 y;
    float32 width;
    float32 height;

    constexpr Rect() noexcept;
    constexpr Rect(float32, float32, float32, float32) noexcept;
    constexpr Rect(Vector2 const&, Vector2 const&) noexcept;
    explicit constexpr Rect(RectInt const&) noexcept;

    constexpr Vector2 Center() const noexcept;
    constexpr Vector2 Position() const noexcept;
    constexpr Vector2 Size() const noexcept;
    constexpr Vector2 Max() const noexcept;
    constexpr Vector2 Min() const noexcept;

    constexpr bool Contains(Vector2 const&) const noexcept;
    constexpr bool Contains(Vector3 const&) const noexcept;
    constexpr bool Intersects(Rect const&) const noexcept;
    constexpr bool Overlaps(Rect const&) const noexcept;

    static constexpr bool Intersect(Rect const&, Rect const&) noexcept;
    static constexpr bool Overlap(Rect const&, Rect const&) noexcept;

    constexpr bool operator==(Rect const&) const noexcept;

    static const Rect zero;
};

inline Rect const Rect::zero = Rect();

constexpr Rect::Rect() noexcept
    : x(0.f), y(0.f), width(0.f), height(0.f)
{
}

constexpr Rect::Rect(float32 inX, float32 inY, float32 inWidth, float32 inHeight) noexcept
    : x(inX), y(inY), width(inWidth), height(inHeight)
{
}

constexpr Rect::Rect(Vector2 const& position, Vector2 const& size) noexcept
    : x(position.x), y(position.y), width(size.x), height(size.y)
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
    return Vector2(x + (width / 2), y + (height / 2));
}

constexpr Vector2 Rect::Position() const noexcept
{
    return Vector2(x, y);
}

constexpr Vector2 Rect::Size() const noexcept
{
    return Vector2(width, height);
}

constexpr Vector2 Rect::Max() const noexcept
{
    return Vector2(x + width, y + height);
}

constexpr Vector2 Rect::Min() const noexcept
{
    return Vector2(x, y);
}

constexpr bool Rect::Contains(Vector2 const& v) const noexcept
{
    return (v.x >= x) && (v.x <= (x + width)) && (v.y >= y) && (v.y <= (y + height));
}

constexpr bool Rect::Contains(Vector3 const& v) const noexcept
{
    return Contains(Vector2(v));
}

constexpr bool Rect::Intersects(Rect const& r) const noexcept
{
    return (r.x < (x + width)) && (x < (r.x + r.width)) &&
        (r.y < (y + height)) && (y < (r.y + r.height));
}

constexpr bool Rect::Overlaps(Rect const& r) const noexcept
{
    return (r.x > x) && (r.y > y) && (r.width < width) && (r.height < height);
}

constexpr bool Rect::Intersect(Rect const& r1, Rect const& r2) noexcept
{
    return r1.Intersects(r2);
}

constexpr bool Rect::Overlap(Rect const& r1, Rect const& r2) noexcept
{
    return r1.Overlaps(r2);
}

constexpr bool Rect::operator==(Rect const& r) const noexcept
{
    return x == r.x && y == r.y && width == r.width && height == r.height;
}

} // namespace mini