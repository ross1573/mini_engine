module mini.metal;

import mini.core;
import mini.graphics;
import mini.apple;
import :render_context;
import :swap_chain;

namespace mini::metal {

RenderContext::RenderContext(MTL::Device* device)
    : m_autoReleasePool(nullptr)
    , m_cmdQueue(nullptr)
    , m_cmdBuffer(nullptr)
    , m_event(nullptr)
    , m_eventValue(0)
{
    ASSERT(device);

    m_cmdQueue = TransferShared(device->newCommandQueue());
    m_event = TransferShared(device->newSharedEvent());
}

bool RenderContext::Initialize()
{
    ENSURE(m_cmdQueue, "failed to create MTLCommandQueue") {
        return false;
    }

    return true;
}

void RenderContext::BeginRender()
{
    m_event->waitUntilSignaledValue(m_eventValue, ~uint64(0));

    m_autoReleasePool = TransferShared(NS::AutoreleasePool::alloc()->init());
    m_cmdBuffer = m_cmdQueue->commandBuffer();
    m_drawable = interface->GetSwapChain()->GetCurrentDrawable();

    MTL::Texture* texture = m_drawable->texture();
    MTL::RenderPassDescriptor* renderPass = MTL::RenderPassDescriptor::renderPassDescriptor();
    MTL::RenderPassColorAttachmentDescriptor* clearColorAttachment =
        renderPass->colorAttachments()->object(0);

    Color clear = Color::Clear();
    MTL::ClearColor clearColor(clear.r, clear.g, clear.b, clear.a);

    clearColorAttachment->setTexture(texture);
    clearColorAttachment->setLoadAction(MTL::LoadActionClear);
    clearColorAttachment->setStoreAction(MTL::StoreActionStore);
    clearColorAttachment->setClearColor(clearColor);
    clearColorAttachment->clearColor();

    m_cmdEncoder = m_cmdBuffer->renderCommandEncoder(renderPass);
}

void RenderContext::EndRender()
{
}

void RenderContext::WaitForIdle()
{
}

void RenderContext::Execute()
{
    m_cmdEncoder->endEncoding();
    m_cmdBuffer->presentDrawable(m_drawable);

    m_eventValue++;
    m_cmdBuffer->encodeSignalEvent(m_event.Get(), m_eventValue);
    m_cmdBuffer->commit();

    m_drawable = nullptr;
    m_cmdEncoder = nullptr;
    m_cmdBuffer = nullptr;
    m_autoReleasePool.Reset();
}

void RenderContext::SetViewport(Rect const&, float32, float32)
{
}

void RenderContext::SetScissorRect(RectInt const&)
{
}

} // namespace mini::metal