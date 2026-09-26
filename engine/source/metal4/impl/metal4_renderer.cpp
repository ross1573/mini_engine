module mini.metal4;

import mini.core;
import mini.graphics;
import mini.apple;
import :renderer;
import :swap_chain;

namespace mini::metal4 {

Renderer::Renderer(Device* device)
    : m_device(device)
    , m_commandAllocatorPool(device)
    , m_eventValue(0)
{
    ASSERT(device);

    m_commandQueue = MakeUnique<CommandQueue>(device);
    m_commandBuffer = MakeUnique<CommandBuffer>(device);
    m_compiler = MakeUnique<Compiler>(device);
    m_library = MakeUnique<ShaderLibrary>(device, "mini.shader");

    ASSERT(m_commandQueue);
    ASSERT(m_commandBuffer);
    ASSERT(m_compiler);
    ASSERT(m_library);

    m_vertexFunction = MakeUnique<ShaderFunction>(m_library.Get(), "VertexMain");
    m_fragmentFunction = MakeUnique<ShaderFunction>(m_library.Get(), "FragmentMain");
    RenderPipelineDescriptor pipelineDesc(m_vertexFunction.Get(), m_fragmentFunction.Get());
    m_renderPipelineState = MakeUnique<RenderPipelineState>(m_compiler.Get(), memory::AddressOf(pipelineDesc));

    ASSERT(m_renderPipelineState);

    m_event = MakeUnique<SharedEvent>(m_device);

    ASSERT(m_event);
}

void Renderer::Render()
{
    [[maybe_unsed]] apple::AutoreleasePool autoreleaesPool;

    m_commandAllocatorPool.Expire(m_eventValue);
    m_event->Wait(m_eventValue);
    ++m_eventValue;

    UniquePtr<CommandAllocator> commandAllocator = m_commandAllocatorPool.Allocate();
    m_commandBuffer->Begin(commandAllocator.Get());

    SwapChain* swapChain = interface->GetSwapChain();
    UniquePtr<Texture> frameTexture = swapChain->FrameTexture();
    graphics::RenderPassTargetAttachment targetAttachment{
        frameTexture.Get(),
        graphics::LoadAction::Clear,
        graphics::StoreAction::Store,
        Color::Clear(),
    };

    graphics::RenderPassDescriptor renderPassDescriptor{
        {targetAttachment},
    };

    // basic triangle pass
    {
        RenderPass renderPass{m_commandBuffer.Get(), renderPassDescriptor};
        renderPass.SetPipelineState(m_renderPipelineState.Get());
        renderPass.DrawPrimitives(graphics::PrimitiveType::Triangle, 0, 3);
    }

    m_commandBuffer->End();
    m_commandAllocatorPool.Pending(MoveArg(commandAllocator), m_eventValue);

    m_commandQueue->Commit(m_commandBuffer.Get());
    m_commandQueue->Signal(m_event.Get(), m_eventValue);
}

void Renderer::WaitForIdle()
{
    [[maybe_unsed]] apple::AutoreleasePool autoreleaesPool;

    m_eventValue++;
    m_commandQueue->Signal(m_event.Get(), m_eventValue);
    m_event->Wait(m_eventValue);
}

} // namespace mini::metal4