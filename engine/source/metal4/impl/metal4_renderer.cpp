module mini.metal4;

import mini.core;
import mini.graphics;
import mini.apple;
import :renderer;
import :swap_chain;

namespace mini::metal4 {

Renderer::Renderer(MTL::Device* device)
    : m_autoReleasePool(nullptr)
    , m_cmdQueue(nullptr)
    , m_cmdBuffer(nullptr)
    , m_event(nullptr)
    , m_eventValue(0)
{
    ASSERT(device);

    m_cmdQueue = TransferShared(device->newMTL4CommandQueue());
    m_cmdBuffer = TransferShared(device->newCommandBuffer());
    m_cmdAllocator = TransferShared(device->newCommandAllocator());
    m_event = TransferShared(device->newSharedEvent());
}

bool Renderer::Initialize()
{
    ENSURE(m_cmdQueue, "failed to create MTL4::CommandQueue") {
        return false;
    }

    ENSURE(m_cmdBuffer, "failed to create MTL4::CommandBuffer") {
        return false;
    }

    ENSURE(m_cmdAllocator, "failed to create MTL4::CommandAllocator") {
        return false;
    }

    ENSURE(m_event, "failed to create MTL::SharedEvent") {
        return false;
    }

    return true;
}

void Renderer::BeginRender()
{
    m_event->waitUntilSignaledValue(m_eventValue, ~uint64(0));

    m_autoReleasePool = TransferShared(NS::AutoreleasePool::alloc()->init());
    m_drawable = interface->GetSwapChain()->GetCurrentDrawable();

    m_cmdAllocator->reset();
    m_cmdBuffer->beginCommandBuffer(m_cmdAllocator.Get());

    MTL4::RenderPassDescriptor* renderPassDescriptor = MTL4::RenderPassDescriptor::alloc()->init();
    MTL::RenderPassColorAttachmentDescriptor* clearColorAttachment =
        renderPassDescriptor->colorAttachments()->object(0);

    Color clear = Color::Clear();
    MTL::ClearColor clearColor(clear.r, clear.g, clear.b, clear.a);
    MTL::Texture* texture = m_drawable->texture();

    clearColorAttachment->setTexture(texture);
    clearColorAttachment->setLoadAction(MTL::LoadActionClear);
    clearColorAttachment->setStoreAction(MTL::StoreActionStore);
    clearColorAttachment->setClearColor(clearColor);
    clearColorAttachment->clearColor();

    m_cmdEncoder = m_cmdBuffer->renderCommandEncoder(renderPassDescriptor);
}

void Renderer::EndRender()
{
    m_cmdEncoder->endEncoding();
    m_cmdBuffer->endCommandBuffer();
}

void Renderer::WaitForIdle()
{
    m_eventValue++;
    m_cmdQueue->signalEvent(m_event.Get(), m_eventValue);
    m_event->waitUntilSignaledValue(m_eventValue, ~uint64(0));
}

void Renderer::Execute()
{
    MTL4::CommandBuffer* bufferPtr = m_cmdBuffer.Get();
    m_cmdQueue->commit(&bufferPtr, 1);

    m_eventValue++;
    m_cmdQueue->signalEvent(m_event.Get(), m_eventValue);

    m_drawable = nullptr;
    m_cmdEncoder = nullptr;
    m_autoReleasePool.Reset();
}

void Renderer::SetViewport(Rect const&, float32, float32)
{
}

void Renderer::SetScissorRect(RectInt const&)
{
}

} // namespace mini::metal4