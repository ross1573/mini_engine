module mini.metal4;

import :render_pass;

namespace mini::metal4 {

RenderPass::RenderPass(MTL4::CommandBuffer* commandBuffer) noexcept
    : m_targetTexture(nullptr)
    , m_commandBuffer(commandBuffer)
    , m_renderCommandEncoder(nullptr)
{
    ASSERT(m_commandBuffer != nullptr);

    MTL4::RenderPassDescriptor* renderPassDescriptor = MTL4::RenderPassDescriptor::alloc();
    ENSURE(renderPassDescriptor != nullptr, "failed to create MTL4::RenderPassDescriptor") {
        m_commandBuffer = nullptr;
        return;
    }

    m_renderPassDescriptor = TransferShared(renderPassDescriptor);
    m_renderPassDescriptor->init();
}

RenderPass::~RenderPass() noexcept
{
    m_commandBuffer = nullptr;
    m_renderCommandEncoder = nullptr;
}

bool RenderPass::Valid() const noexcept
{
    return m_commandBuffer != nullptr;
}

bool RenderPass::Active() const noexcept
{
    return m_renderCommandEncoder != nullptr;
}

void RenderPass::Begin(MTL::Texture* targetTexture, Color const& color) noexcept
{
    m_targetTexture = targetTexture;

    ASSERT(m_commandBuffer != nullptr);
    ASSERT(m_targetTexture != nullptr);

    NS::AutoreleasePool* autoReleasePool = NS::AutoreleasePool::alloc();
    ENSURE(autoReleasePool != nullptr, "failed to allocate NS::AutoreleasePool") {
        return;
    }

    m_autoReleasePool = TransferShared(autoReleasePool);
    m_autoReleasePool->init();

    MTL::RenderPassColorAttachmentDescriptor* colorAttachment = m_renderPassDescriptor->colorAttachments()->object(0);
    MTL::ClearColor clearColor(static_cast<double>(color.r),
                               static_cast<double>(color.g),
                               static_cast<double>(color.b),
                               static_cast<double>(color.a));

    colorAttachment->setTexture(m_targetTexture);
    colorAttachment->setLoadAction(MTL::LoadActionClear);
    colorAttachment->setStoreAction(MTL::StoreActionStore);
    colorAttachment->setClearColor(clearColor);
    colorAttachment->clearColor();

    MTL4::RenderPassDescriptor* renderPassDescriptor = m_renderPassDescriptor.Get();
    MTL4::RenderCommandEncoder* renderCommandEncoder = m_commandBuffer->renderCommandEncoder(renderPassDescriptor);

    ENSURE(renderCommandEncoder != nullptr, "failed to create MTL4::RenderCommandEncoder") {
        return;
    }

    m_renderCommandEncoder = renderCommandEncoder;
}

void RenderPass::End() noexcept
{
    ASSERT(m_renderCommandEncoder != nullptr);

    m_renderCommandEncoder->endEncoding();
    m_renderCommandEncoder = nullptr;

    m_autoReleasePool.Reset();
}

void RenderPass::SetViewport(Rect const& rect, float32 near, float32 far) noexcept
{
    ASSERT(m_renderCommandEncoder != nullptr);

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
    ASSERT(m_renderCommandEncoder != nullptr);

    MTL::ScissorRect ScissorRect = {
        .x = static_cast<NS::UInteger>(rect.x),
        .y = static_cast<NS::UInteger>(rect.y),
        .width = static_cast<NS::UInteger>(rect.width),
        .height = static_cast<NS::UInteger>(rect.height),
    };

    m_renderCommandEncoder->setScissorRect(ScissorRect);
}

} // namespace mini::metal4