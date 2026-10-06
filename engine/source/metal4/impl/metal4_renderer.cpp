module mini.metal4;

import mini.core;
import mini.graphics;
import mini.apple;
import :renderer;
import :swap_chain;
import :common;
import :log;

namespace mini::metal4 {

Renderer::Renderer(PtrView<Device> device)
    : m_device(device)
    , m_commandAllocatorPool(device)
{
    ASSERT(device);

    m_commandQueue = MakeUnique<CommandQueue>(device);
    m_commandBuffer = MakeUnique<CommandBuffer>(device);
    m_compiler = MakeUnique<Compiler>(device);
    m_library = MakeUnique<ShaderLibrary>(device, "mini.metal.shader");

    ASSERT(m_commandQueue);
    ASSERT(m_commandBuffer);
    ASSERT(m_compiler);
    ASSERT(m_library);

    m_event = MakeUnique<SharedEvent>(m_device);
    m_eventValue = 0;
    m_frameValue = 0;

    for (size_t i = 0; i < options::bufferCount; ++i) {
        m_eventQueue.PushBack(uint64{0});
    }

    ASSERT(m_event);
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

    m_frameValue = m_eventQueue.PopFirst();
    m_event->Wait(m_frameValue);
    ++m_eventValue;

    m_commandAllocatorPool.Expire(m_frameValue);

    SwapChain* swapChain = interface->GetSwapChain();
    Drawable* drawable = swapChain->Drawable();
    UniquePtr<CommandAllocator> commandAllocator = m_commandAllocatorPool.Allocate();
    m_commandBuffer->Begin(commandAllocator);

    UniquePtr<Texture> frameTexture = drawable->FrameTexture();
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
        Rect rect{0, 0, static_cast<float>(options::width), static_cast<float>(options::height)};
        UniquePtr<RenderCommandEncoder> renderEncoder =
            m_commandBuffer->AllocateRenderCommandEncoder(renderPassDescriptor);

        renderEncoder->SetViewport(rect, 0.f, 1.f);
        renderEncoder->SetPipelineState(m_renderPipelineState);
        renderEncoder->DrawPrimitives(graphics::PrimitiveType::Triangle, 0, 3);
    }

    m_commandBuffer->End();
    m_commandAllocatorPool.Pending(MoveArg(commandAllocator), m_eventValue);

    m_commandQueue->Commit(m_commandBuffer);
    m_commandQueue->Signal(m_event, m_eventValue);
    m_eventQueue.PushBack(m_eventValue);
}

void Renderer::WaitForIdle()
{
    [[maybe_unsed]] apple::AutoreleasePool autoreleaesPool;

    m_eventValue++;
    m_commandQueue->Signal(m_event, m_eventValue);
    m_event->Wait(m_eventValue);
}

void Renderer::HandleRenderError(PtrView<NS::Error> error)
{
    Renderer* renderer = interface->GetRenderer();
    ASSERT(renderer);
    ENSURE(error == nullptr,
           error,
           "failed on event value {}. (rendering: {}, signaled: {})",
           renderer->m_eventValue,
           renderer->m_frameValue,
           renderer->m_event->SignaledValue()) { }
}

} // namespace mini::metal4