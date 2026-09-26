module mini.metal4;

import mini.core;
import mini.graphics;
import mini.apple;
import :log;
import :common;
import :render_pass;

namespace mini::metal4 {

SharedPtr<MTL4::RenderPassDescriptor> MTLRenderPassDescriptor(
    graphics::RenderPassDescriptor const& renderPassDescriptor)
{
    SharedPtr<MTL4::RenderPassDescriptor> desc = TransferShared(MTL4::RenderPassDescriptor::alloc());
    ENSURE(desc.Valid(), "failed to allocate MTL4::RenderPassDescriptor") {
        return nullptr;
    }

    desc->init();

    Array<graphics::RenderPassTargetAttachment> const& targetAttachments = renderPassDescriptor.targetAttachments;
    MTL::RenderPassColorAttachmentDescriptorArray* colorDescriptors = desc->colorAttachments();

    for (size_t i = 0; i < targetAttachments.Size(); ++i) {
        graphics::RenderPassTargetAttachment const& targetAttachment = targetAttachments[i];
        Texture* texture = static_cast<Texture*>(targetAttachment.texture);
        if (texture == nullptr) {
            continue;
        }

        MTL::RenderPassColorAttachmentDescriptor* colorDescriptor = colorDescriptors->object(0);
        colorDescriptor->setTexture(texture->MTLTexture());
        colorDescriptor->setClearColor(MTLClearColor(targetAttachment.clearColor));
        colorDescriptor->setLoadAction(MTLLoadAction(targetAttachment.loadAction));
        colorDescriptor->setStoreAction(MTLStoreAction(targetAttachment.storeAction));
    }

    graphics::RenderPassDepthAttachment const& depthAttachment = renderPassDescriptor.depthAttachment;
    Texture* depthTexture = static_cast<Texture*>(depthAttachment.texture);
    if (depthTexture != nullptr) {
        MTL::RenderPassDepthAttachmentDescriptor* depthDescriptor = desc->depthAttachment();
        depthDescriptor->setTexture(depthTexture->MTLTexture());
        depthDescriptor->setClearDepth(static_cast<float64>(depthAttachment.clearDepth));
        depthDescriptor->setLoadAction(MTLLoadAction(depthAttachment.loadAction));
        depthDescriptor->setStoreAction(MTLStoreAction(depthAttachment.storeAction));
    }

    graphics::RenderPassStencilAttachment const& stencilAttachment = renderPassDescriptor.stencilAttachment;
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

RenderPass::RenderPass(CommandBuffer* commandBuffer, graphics::RenderPassDescriptor const& descriptor) noexcept
{
    ASSERT(commandBuffer);

    SharedPtr<MTL4::RenderPassDescriptor> desc = MTLRenderPassDescriptor(descriptor);
    ENSURE(desc, "failed to create MTL4::RenderPassDescriptor") {
        return;
    }

    MTL4::CommandBuffer* mtlCommandBuffer = commandBuffer->MTLCommandBuffer();
    MTL4::RenderCommandEncoder* mtlRenderEncoder = mtlCommandBuffer->renderCommandEncoder(desc.Get());
    ENSURE(mtlRenderEncoder, "failed to create renderpass") {
        return;
    }

    m_commandBuffer = commandBuffer;
    m_renderCommandEncoder = mtlRenderEncoder;
}

RenderPass::~RenderPass() noexcept
{
    if (m_renderCommandEncoder != nullptr) [[likely]] {
        m_renderCommandEncoder->endEncoding();
    }

    m_renderCommandEncoder = nullptr;
    m_commandBuffer = nullptr;
}

bool RenderPass::Valid() const noexcept
{
    return m_commandBuffer != nullptr && m_renderCommandEncoder != nullptr;
}

void RenderPass::DrawPrimitives(graphics::PrimitiveType primitiveType, uint64 vertexStart, uint64 vertexCount)
{
    ASSERT(Valid());

    MTL::PrimitiveType mtlPrimitive = MTLPrimitiveType(primitiveType);
    // MTL::RenderStages renderStages = MTL::RenderStageVertex;

    // m_renderCommandEncoder->setArgumentTable(m_argumentTable.Get(), renderStages);
    m_renderCommandEncoder->drawPrimitives(mtlPrimitive, vertexStart, vertexCount);
}

void RenderPass::SetPipelineState(RenderPipelineState* state)
{
    ASSERT(Valid());
    ASSERT(state);

    m_renderCommandEncoder->setRenderPipelineState(state->MTLRenderPipelineState());
}

void RenderPass::SetViewport(Rect const& rect, float32 near, float32 far) noexcept
{
    ASSERT(Valid());

    MTL::Viewport viewport = {
        .originX = static_cast<double>(rect.x),
        .originY = static_cast<double>(rect.y),
        .width = static_cast<double>(rect.width),
        .height = static_cast<double>(rect.height),
        .znear = static_cast<double>(near),
        .zfar = static_cast<double>(far),
    };

    m_renderCommandEncoder->setViewport(viewport);
}

void RenderPass::SetScissorRect(RectInt const& rect) noexcept
{
    ASSERT(Valid());

    MTL::ScissorRect ScissorRect = {
        .x = static_cast<NS::UInteger>(rect.x),
        .y = static_cast<NS::UInteger>(rect.y),
        .width = static_cast<NS::UInteger>(rect.width),
        .height = static_cast<NS::UInteger>(rect.height),
    };

    m_renderCommandEncoder->setScissorRect(ScissorRect);
}

} // namespace mini::metal4