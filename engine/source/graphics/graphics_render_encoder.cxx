export module mini.graphics:render_encoder;

import mini.core;

namespace mini::graphics {

export enum class PrimitiveType : byte {
    Point = 0,
    Line = 1,
    Triangle = 2,
};

export struct GRAPHICS_API ScissorRect {
public:
    uint32 x;
    uint32 y;
    uint32 width;
    uint32 height;

public:
    constexpr ScissorRect() noexcept = default;

    constexpr ScissorRect(uint32 x, uint32 y, uint32 width, uint32 height) noexcept
        : x(x)
        , y(y)
        , width(width)
        , height(height)
    {
    }
};

export struct GRAPHICS_API Viewport {
public:
    float32 x;
    float32 y;
    float32 width;
    float32 height;
    float32 near;
    float32 far;

public:
    constexpr Viewport() = default;

    constexpr Viewport(float32 x, float32 y, float32 width, float32 height, float32 near, float32 far)
        : x(x)
        , y(y)
        , width(width)
        , height(height)
        , near(near)
        , far(far)
    {
    }

    constexpr Viewport(Vector2 position, Vector2 size, Vector2 depth)
        : x(position.x)
        , y(position.y)
        , width(size.x)
        , height(size.y)
        , near(depth.x)
        , far(depth.y)
    {
    }
};

} // namespace mini::graphics