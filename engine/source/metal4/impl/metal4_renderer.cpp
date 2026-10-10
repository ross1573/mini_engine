module mini.metal4;

import mini.core;
import mini.graphics;
import mini.apple;
import :common;
import :event;
import :command_encoder;
import :renderer;
import :swap_chain;

namespace mini::metal4 {

Renderer::Renderer(PtrView<Device> device)
    : m_device(device)
{
    ASSERT(device);

    m_commandQueue = MakeUnique<CommandQueue>(device);
    m_commandBufferAllocator =
        MakeUnique<CommandBufferAllocator>(device, m_commandQueue, size_t{1}, options::bufferCount);

    m_compiler = MakeUnique<Compiler>(device);
    m_library = MakeUnique<ShaderLibrary>(device, "mini.metal.shader");

    m_frameValue = 0;
    for (size_t i = 0; i < options::bufferCount; ++i) {
        m_frameQueue.PushBack(uint64{0});
    }
}

void Renderer::Prepare()
{
    SwapChain* swapChain = interface->GetSwapChain();
    m_commandQueue->AddResidencySet(swapChain->CAMetalLayer()->residencySet());

    m_vertexFunction = MakeUnique<ShaderFunction>(m_library, "VertexMain");
    m_fragmentFunction = MakeUnique<ShaderFunction>(m_library, "FragmentMain");

    graphics::RenderPipelineDescriptor pipelineDesc{
        {graphics::RenderPipelineTargetAttachment(graphics::PixelFormat::BGRA8unorm)},
        m_vertexFunction,
        m_fragmentFunction,
    };

    m_renderPipelineState = MakeUnique<RenderPipelineState>(m_compiler, pipelineDesc);
    ASSERT(m_renderPipelineState);
}

void Renderer::Render()
{
    [[maybe_unsed]] apple::AutoreleasePool autoreleaesPool;

    m_frameValue = m_frameQueue.PopFirst();
    m_commandBufferAllocator->Wait(m_frameValue);

    SwapChain* swapChain = interface->GetSwapChain();
    Drawable* drawable = swapChain->Drawable();
    UniquePtr<Texture> frameTexture = drawable->FrameTexture();
    UniquePtr<CommandBuffer> commandBuffer = m_commandBufferAllocator->Allocate();

    graphics::RenderScopeTargetAttachment targetAttachment{
        frameTexture,
        graphics::LoadAction::Clear,
        graphics::StoreAction::Store,
        Color::Clear(),
    };

    graphics::RenderScopeDescriptor renderPassDescriptor{
        {targetAttachment},
    };

    // basic triangle pass
    {
        UniquePtr<RenderCommandEncoder> renderEncoder =
            commandBuffer->AllocateRenderCommandEncoder(renderPassDescriptor);

        renderEncoder->SetPipelineState(m_renderPipelineState);
        renderEncoder->DrawPrimitives(graphics::PrimitiveType::Triangle, 0, 3);
    }

    size_t frameValue = m_commandBufferAllocator->Commit(MoveArg(commandBuffer));
    m_frameQueue.PushBack(frameValue);
}

void Renderer::WaitForIdle()
{
    [[maybe_unsed]] apple::AutoreleasePool autoreleaesPool;

    // TODO
    SharedEvent sharedEvent(m_device);
    m_commandQueue->Signal(memory::AddressOf(sharedEvent), 1);
    sharedEvent.Wait(1);
}

void Renderer::HandleRenderError(PtrView<NS::Error> error)
{
    Renderer* renderer = interface->GetRenderer();
    ASSERT(renderer);
    ENSURE(error == nullptr,
           error,
           "failed on event value {}. (signaled: {}, presented: {})",
           renderer->m_commandBufferAllocator->CommittedValue(),
           renderer->m_commandBufferAllocator->SignaledValue(),
           renderer->m_frameValue) { }
}

} // namespace mini::metal4