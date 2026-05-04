module mini.metal4;

import mini.core;
import mini.graphics;
import mini.apple;
import :renderer;
import :swap_chain;
import :render_pass;

namespace mini::metal4 {

Renderer::Renderer(MTL::Device* device)
    : m_autoReleasePool(nullptr)
    , m_commandQueue(nullptr)
    , m_commandBuffer(nullptr)
    , m_commandAllocator(nullptr)
    , m_eventValue(0)
{
    ASSERT(device);

    m_commandQueue = TransferShared(device->newMTL4CommandQueue());
    m_commandBuffer = TransferShared(device->newCommandBuffer());
    m_commandAllocator = TransferShared(device->newCommandAllocator());
    m_event = TransferShared(device->newSharedEvent());
}

bool Renderer::Initialize()
{
    ENSURE(m_commandQueue, "failed to create MTL4::CommandQueue") return false;
    ENSURE(m_commandBuffer, "failed to create MTL4::CommandBuffer") return false;
    ENSURE(m_commandAllocator, "failed to create MTL4::CommandAllocator") return false;
    ENSURE(m_event, "failed to create MTL::SharedEvent") return false;

    return true;
}

void Renderer::BeginRender()
{
    m_event->waitUntilSignaledValue(m_eventValue, ~uint64(0));

    NS::AutoreleasePool* autoReleasePool = NS::AutoreleasePool::alloc();
    ENSURE(autoReleasePool != nullptr, "failed to allocate NS::AutoreleasePool") {
        return;
    }

    m_autoReleasePool = TransferShared(autoReleasePool);
    m_autoReleasePool->init();

    CA::MetalDrawable* drawable = interface->GetSwapChain()->GetCurrentDrawable();
    MTL::Texture* targetTexture = drawable->texture();

    m_commandAllocator->reset();
    m_commandBuffer->beginCommandBuffer(m_commandAllocator.Get());

    RenderPass renderPass(m_commandBuffer.Get());
    renderPass.Begin(targetTexture, Color::Clear());
    m_renderPasses.Push(MoveArg(renderPass));
}

void Renderer::EndRender()
{
    for (RenderPass& renderPass : m_renderPasses) {
        renderPass.End();
    }

    m_renderPasses.Clear();
    m_commandBuffer->endCommandBuffer();
}

void Renderer::WaitForIdle()
{
    m_eventValue++;
    m_commandQueue->signalEvent(m_event.Get(), m_eventValue);
    m_event->waitUntilSignaledValue(m_eventValue, ~uint64(0));
}

void Renderer::Execute()
{
    MTL4::CommandBuffer* commandBuffer = m_commandBuffer.Get();
    m_commandQueue->commit(&commandBuffer, 1);

    m_eventValue++;
    m_commandQueue->signalEvent(m_event.Get(), m_eventValue);

    m_autoReleasePool.Reset();
}

void Renderer::SetViewport(Rect const& rect, float32 near, float32 far)
{
    for (RenderPass& renderPass : m_renderPasses) {
        renderPass.SetViewport(rect, near, far);
    }
}

void Renderer::SetScissorRect(RectInt const& rect)
{
    for (RenderPass& renderPass : m_renderPasses) {
        renderPass.SetScissorRect(rect);
    }
}

} // namespace mini::metal4