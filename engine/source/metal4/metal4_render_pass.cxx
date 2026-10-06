module;

#include <Metal/MTL4ArgumentTable.hpp>
#include <Metal/MTL4RenderPass.hpp>

export module mini.metal4:render_pass;

import mini.core;
import mini.graphics;
import :buffer;
import :texture;
import :command_buffer;
import :render_pipeline;

export namespace MTL {

using MTL::ClearColor;
using MTL::LoadAction;
using MTL::StoreAction;

using MTL::RenderPassColorAttachmentDescriptor;
using MTL::RenderPassColorAttachmentDescriptorArray;
using MTL::RenderPassDepthAttachmentDescriptor;
using MTL::RenderPassDescriptor;
using MTL::RenderPassStencilAttachmentDescriptor;

} // namespace MTL

export namespace MTL4 {

using MTL4::RenderPassDescriptor;

} // namespace MTL4

namespace mini::metal4 {

export METAL4_API constexpr MTL::ClearColor MTLClearColor(Color color) noexcept
{
    return MTL::ClearColor{
        static_cast<double>(color.r),
        static_cast<double>(color.g),
        static_cast<double>(color.b),
        static_cast<double>(color.a),
    };
}

export METAL4_API constexpr MTL::LoadAction MTLLoadAction(graphics::LoadAction loadAction) noexcept
{
    switch (loadAction) {
        case graphics::LoadAction::Undefined: return MTL::LoadActionDontCare;
        case graphics::LoadAction::Load:      return MTL::LoadActionLoad;
        case graphics::LoadAction::Clear:     return MTL::LoadActionClear;
    }

    ASSERT(loadAction == graphics::LoadAction::Undefined, "invalid load action {}.", static_cast<byte>(loadAction));
    return MTL::LoadActionDontCare;
}

export METAL4_API constexpr MTL::StoreAction MTLStoreAction(graphics::StoreAction storeAction) noexcept
{
    switch (storeAction) {
        case graphics::StoreAction::Undefined: return MTL::StoreActionDontCare;
        case graphics::StoreAction::Store:     return MTL::StoreActionStore;
    }

    ASSERT(storeAction == graphics::StoreAction::Undefined, "invalid store action {}.", static_cast<byte>(storeAction));
    return MTL::StoreActionDontCare;
}

export SharedPtr<MTL4::RenderPassDescriptor> MTLRenderPassDescriptor(
    graphics::RenderScopeDescriptor const& renderScopeDescriptor)
{
    SharedPtr<MTL4::RenderPassDescriptor> desc = TransferShared(MTL4::RenderPassDescriptor::alloc());
    ENSURE(desc.Valid(), "failed to allocate MTL4::RenderPassDescriptor") {
        return nullptr;
    }

    desc->init();

    Array<graphics::RenderScopeTargetAttachment> const& targetAttachments = renderScopeDescriptor.targetAttachments;
    MTL::RenderPassColorAttachmentDescriptorArray* colorDescriptors = desc->colorAttachments();

    for (size_t i = 0; i < targetAttachments.Size(); ++i) {
        graphics::RenderScopeTargetAttachment const& targetAttachment = targetAttachments[i];
        Texture* texture = static_cast<Texture*>(targetAttachment.texture);
        if (texture == nullptr) {
            continue;
        }

        MTL::RenderPassColorAttachmentDescriptor* colorDescriptor = colorDescriptors->object(i);
        colorDescriptor->setTexture(texture->MTLTexture());
        colorDescriptor->setClearColor(MTLClearColor(targetAttachment.clearColor));
        colorDescriptor->setLoadAction(MTLLoadAction(targetAttachment.loadAction));
        colorDescriptor->setStoreAction(MTLStoreAction(targetAttachment.storeAction));
    }

    graphics::RenderScopeDepthAttachment const& depthAttachment = renderScopeDescriptor.depthAttachment;
    Texture* depthTexture = static_cast<Texture*>(depthAttachment.texture);
    if (depthTexture != nullptr) {
        MTL::RenderPassDepthAttachmentDescriptor* depthDescriptor = desc->depthAttachment();
        depthDescriptor->setTexture(depthTexture->MTLTexture());
        depthDescriptor->setClearDepth(static_cast<float64>(depthAttachment.clearDepth));
        depthDescriptor->setLoadAction(MTLLoadAction(depthAttachment.loadAction));
        depthDescriptor->setStoreAction(MTLStoreAction(depthAttachment.storeAction));
    }

    graphics::RenderScopeStencilAttachment const& stencilAttachment = renderScopeDescriptor.stencilAttachment;
    Texture* stencilTexture = static_cast<Texture*>(stencilAttachment.texture);
    if (stencilTexture != nullptr) {
        MTL::RenderPassStencilAttachmentDescriptor* stencilDescriptor = desc->stencilAttachment();
        stencilDescriptor->setTexture(stencilTexture->MTLTexture());
        stencilDescriptor->setClearStencil(stencilAttachment.clearStencil);
        stencilDescriptor->setLoadAction(MTLLoadAction(stencilAttachment.loadAction));
        stencilDescriptor->setStoreAction(MTLStoreAction(stencilAttachment.storeAction));
    }

    return desc;
}

} // namespace mini::metal4