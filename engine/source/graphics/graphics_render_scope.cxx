export module mini.graphics:render_scope;

import mini.core;

namespace mini::graphics {

class Texture;

} // namespace mini::graphics

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

export struct GRAPHICS_API RenderScopeAttachment {
public:
    Texture* texture;
    LoadAction loadAction;
    StoreAction storeAction;

public:
    RenderScopeAttachment()
        : texture(nullptr)
        , loadAction(LoadAction::Undefined)
        , storeAction(StoreAction::Undefined)
    {
    }

    RenderScopeAttachment(PtrView<Texture> texture, LoadAction loadAction, StoreAction storeAction) noexcept
        : texture(texture)
        , loadAction(loadAction)
        , storeAction(storeAction)
    {
    }
};

export struct GRAPHICS_API RenderScopeTargetAttachment : public RenderScopeAttachment {
public:
    Color clearColor = Color::Clear();

public:
    RenderScopeTargetAttachment() noexcept = default;

    RenderScopeTargetAttachment(PtrView<Texture> texture) noexcept
        : RenderScopeAttachment(texture, LoadAction::Undefined, StoreAction::Store)
    {
    }

    RenderScopeTargetAttachment(PtrView<Texture> texture, LoadAction loadAction, StoreAction storeAction) noexcept
        : RenderScopeAttachment(texture, loadAction, storeAction)
    {
    }

    RenderScopeTargetAttachment(PtrView<Texture> texture,
                                LoadAction loadAction,
                                StoreAction storeAction,
                                Color clearColor) noexcept
        : RenderScopeAttachment(texture, loadAction, storeAction)
        , clearColor(clearColor)
    {
    }
};

export struct GRAPHICS_API RenderScopeDepthAttachment : public RenderScopeAttachment {
public:
    float32 clearDepth = 1.0f;

public:
    RenderScopeDepthAttachment() noexcept = default;

    RenderScopeDepthAttachment(PtrView<Texture> texture) noexcept
        : RenderScopeAttachment(texture, LoadAction::Clear, StoreAction::Undefined)
    {
    }

    RenderScopeDepthAttachment(PtrView<Texture> texture, LoadAction loadAction, StoreAction storeAction) noexcept
        : RenderScopeAttachment(texture, loadAction, storeAction)
    {
    }

    RenderScopeDepthAttachment(PtrView<Texture> texture,
                               LoadAction loadAction,
                               StoreAction storeAction,
                               float32 clearDepth) noexcept
        : RenderScopeAttachment(texture, loadAction, storeAction)
        , clearDepth(clearDepth)
    {
    }
};

export struct GRAPHICS_API RenderScopeStencilAttachment : public RenderScopeAttachment {
public:
    uint32 clearStencil = 0;

public:
    RenderScopeStencilAttachment() noexcept = default;

    RenderScopeStencilAttachment(PtrView<Texture> texture) noexcept
        : RenderScopeAttachment(texture, LoadAction::Clear, StoreAction::Undefined)
    {
    }

    RenderScopeStencilAttachment(PtrView<Texture> texture, LoadAction loadAction, StoreAction storeAction) noexcept
        : RenderScopeAttachment(texture, loadAction, storeAction)
    {
    }

    RenderScopeStencilAttachment(PtrView<Texture> texture,
                                 LoadAction loadAction,
                                 StoreAction storeAction,
                                 uint32 clearStencil) noexcept
        : RenderScopeAttachment(texture, loadAction, storeAction)
        , clearStencil(clearStencil)
    {
    }
};

export struct GRAPHICS_API RenderScopeDescriptor {
public:
    Array<RenderScopeTargetAttachment> targetAttachments;
    RenderScopeDepthAttachment depthAttachment;
    RenderScopeStencilAttachment stencilAttachment;

public:
    RenderScopeDescriptor() noexcept = default;

    RenderScopeDescriptor(ArrayView<RenderScopeTargetAttachment> targetAttachments)
        : targetAttachments(targetAttachments)
    {
    }

    RenderScopeDescriptor(ArrayView<RenderScopeTargetAttachment> targetAttachments,
                          RenderScopeDepthAttachment depthAttachment,
                          RenderScopeStencilAttachment stencilAttachment)
        : targetAttachments(targetAttachments)
        , depthAttachment(depthAttachment)
        , stencilAttachment(stencilAttachment)
    {
    }
};

} // namespace mini::graphics