export module mini.graphics:render_pass;

import mini.core;
import :texture;

namespace mini::graphics {

export enum class LoadAction : byte {
    Undefined = 0,
    Load,
    Clear,
};

export enum class StoreAction : byte {
    Undefined = 0,
    Store,
};

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

export struct GRAPHICS_API RenderPassAttachment {
public:
    Texture* texture;
    LoadAction loadAction;
    StoreAction storeAction;

public:
    RenderPassAttachment()
        : texture(nullptr)
        , loadAction(LoadAction::Undefined)
        , storeAction(StoreAction::Undefined)
    {
    }

    RenderPassAttachment(Texture* texture, LoadAction loadAction, StoreAction storeAction) noexcept
        : texture(texture)
        , loadAction(loadAction)
        , storeAction(storeAction)
    {
    }
};

export struct GRAPHICS_API RenderPassTargetAttachment : public RenderPassAttachment {
public:
    Color clearColor = Color::Clear();

public:
    RenderPassTargetAttachment() noexcept = default;

    RenderPassTargetAttachment(Texture* texture) noexcept
        : RenderPassAttachment(texture, LoadAction::Undefined, StoreAction::Store)
    {
    }

    RenderPassTargetAttachment(Texture* texture, LoadAction loadAction, StoreAction storeAction) noexcept
        : RenderPassAttachment(texture, loadAction, storeAction)
    {
    }

    RenderPassTargetAttachment(Texture* texture,
                               LoadAction loadAction,
                               StoreAction storeAction,
                               Color clearColor) noexcept
        : RenderPassAttachment(texture, loadAction, storeAction)
        , clearColor(clearColor)
    {
    }
};

export struct GRAPHICS_API RenderPassDepthAttachment : public RenderPassAttachment {
public:
    float32 clearDepth = 1.0f;

public:
    RenderPassDepthAttachment() noexcept = default;

    RenderPassDepthAttachment(Texture* texture) noexcept
        : RenderPassAttachment(texture, LoadAction::Clear, StoreAction::Undefined)
    {
    }

    RenderPassDepthAttachment(Texture* texture, LoadAction loadAction, StoreAction storeAction) noexcept
        : RenderPassAttachment(texture, loadAction, storeAction)
    {
    }

    RenderPassDepthAttachment(Texture* texture,
                              LoadAction loadAction,
                              StoreAction storeAction,
                              float32 clearDepth) noexcept
        : RenderPassAttachment(texture, loadAction, storeAction)
        , clearDepth(clearDepth)
    {
    }
};

export struct GRAPHICS_API RenderPassStencilAttachment : public RenderPassAttachment {
public:
    uint32 clearStencil = 0;

public:
    RenderPassStencilAttachment() noexcept = default;

    RenderPassStencilAttachment(Texture* texture) noexcept
        : RenderPassAttachment(texture, LoadAction::Clear, StoreAction::Undefined)
    {
    }

    RenderPassStencilAttachment(Texture* texture, LoadAction loadAction, StoreAction storeAction) noexcept
        : RenderPassAttachment(texture, loadAction, storeAction)
    {
    }

    RenderPassStencilAttachment(Texture* texture,
                                LoadAction loadAction,
                                StoreAction storeAction,
                                uint32 clearStencil) noexcept
        : RenderPassAttachment(texture, loadAction, storeAction)
        , clearStencil(clearStencil)
    {
    }
};

export struct GRAPHICS_API RenderPassDescriptor {
public:
    Array<RenderPassTargetAttachment> targetAttachments;
    RenderPassDepthAttachment depthAttachment;
    RenderPassStencilAttachment stencilAttachment;

public:
    RenderPassDescriptor() noexcept = default;

    RenderPassDescriptor(ArrayView<RenderPassTargetAttachment> targetAttachments)
        : targetAttachments(targetAttachments)
    {
    }

    RenderPassDescriptor(ArrayView<RenderPassTargetAttachment> targetAttachments,
                         RenderPassDepthAttachment depthAttachment,
                         RenderPassStencilAttachment stencilAttachment)
        : targetAttachments(targetAttachments)
        , depthAttachment(depthAttachment)
        , stencilAttachment(stencilAttachment)
    {
    }
};

} // namespace mini::graphics