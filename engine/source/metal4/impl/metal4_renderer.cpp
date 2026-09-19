module mini.metal4;

import mini.core;
import mini.graphics;
import mini.apple;
import :renderer;
import :swap_chain;
import :render_pass;

namespace mini::metal4 {

Renderer::Renderer(Device const& device)
    : m_autoReleasePool(nullptr)
    , m_commandQueue(nullptr)
    , m_commandBuffer(nullptr)
    , m_commandAllocator(nullptr)
    , m_compiler(device)
    , m_library(device, "mini.shader")
    , m_renderPasses(1)
    , m_renderPipelineStates(1)
    , m_eventValue(0)
{
    ASSERT(device);

    m_commandQueue = TransferShared(device->newMTL4CommandQueue());
    m_commandBuffer = TransferShared(device->newCommandBuffer());
    m_commandAllocator = TransferShared(device->newCommandAllocator());
    m_event = TransferShared(device->newSharedEvent());

    ENSURE(m_commandBuffer) {
        return;
    }

    RenderPass renderPass(device, m_commandBuffer.Get());
    m_renderPasses.PushBack(MoveArg(renderPass));

    ShaderFunction vertexFunction(m_library, "VertexMain");
    ShaderFunction fragmentFunction(m_library, "FragmentMain");
    RenderPipelineDescriptor pipelineDesc(MoveArg(vertexFunction), MoveArg(fragmentFunction));
    RenderPipelineState pipelineState(m_compiler, pipelineDesc);
    m_renderPipelineStates.PushBack(MoveArg(pipelineState));
}

void Renderer::Render()
{
    m_event->waitUntilSignaledValue(m_eventValue, ~uint64{ 0 });

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

    for (RenderPass& renderPass : m_renderPasses) {
        renderPass.Begin(targetTexture, Color::Clear());
        renderPass.SetPipelineState(*m_renderPipelineStates.Begin());
        renderPass.DrawPrimitives(graphics::PrimitiveType::Triangle, 0, 3);
        renderPass.End();
    }

    m_commandBuffer->endCommandBuffer();
}

void Renderer::WaitForIdle()
{
    m_eventValue++;
    m_commandQueue->signalEvent(m_event.Get(), m_eventValue);
    m_event->waitUntilSignaledValue(m_eventValue, ~uint64{ 0 });
}

void Renderer::Execute()
{
    m_eventValue++;

    CA::MetalDrawable* drawable = interface->GetSwapChain()->GetCurrentDrawable();
    m_commandQueue->wait(drawable);

    MTL4::CommandBuffer* commandBuffer = m_commandBuffer.Get();
    m_commandQueue->commit(&commandBuffer, 1);

    m_commandQueue->signalEvent(m_event.Get(), m_eventValue);
    m_commandQueue->signalDrawable(drawable);

    m_autoReleasePool.Reset();
}

} // namespace mini::metal4