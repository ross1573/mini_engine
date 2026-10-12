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

    m_commandQueue = makeUnique<CommandQueue>(device);
    m_commandBufferAllocator =
        makeUnique<CommandBufferAllocator>(device, m_commandQueue, size_t{1}, options::bufferCount);

    m_compiler = makeUnique<Compiler>(device);
    m_library = makeUnique<ShaderLibrary>(device, "mini.metal.shader");

    m_frameValue = 0;
    for (size_t i = 0; i < options::bufferCount; ++i) {
        m_frameQueue.pushBack(uint64{0});
    }
}

void Renderer::prepare()
{
    SwapChain* swapChain = interface->swapChain();
    m_commandQueue->addResidencySet(swapChain->caMetalLayer()->residencySet());

    m_vertexFunction = makeUnique<ShaderFunction>(m_library, "VertexMain");
    m_fragmentFunction = makeUnique<ShaderFunction>(m_library, "FragmentMain");

    graphics::RenderPipelineDescriptor pipelineDesc{
        {graphics::RenderPipelineTargetAttachment(graphics::PixelFormat::BGRA8unorm)},
        m_vertexFunction,
        m_fragmentFunction,
    };

    m_renderPipelineState = makeUnique<RenderPipelineState>(m_compiler, pipelineDesc);
    ASSERT(m_renderPipelineState);
}

void Renderer::render()
{
    [[maybe_unsed]] apple::AutoreleasePool autoreleaesPool;

    m_frameValue = m_frameQueue.popFirst();
    m_commandBufferAllocator->wait(m_frameValue);

    SwapChain* swapChain = interface->swapChain();
    Drawable* drawable = swapChain->drawable();
    UniquePtr<Texture> frameTexture = drawable->frameTexture();
    UniquePtr<CommandBuffer> commandBuffer = m_commandBufferAllocator->allocate();

    graphics::RenderScopeTargetAttachment targetAttachment{
        frameTexture,
        graphics::LoadAction::clear,
        graphics::StoreAction::store,
        Color::clear(),
    };

    graphics::RenderScopeDescriptor renderPassDescriptor{
        {targetAttachment},
    };

    // basic triangle pass
    {
        UniquePtr<RenderCommandEncoder> renderEncoder = commandBuffer->renderCommandEncoder(renderPassDescriptor);

        renderEncoder->setPipelineState(m_renderPipelineState);
        renderEncoder->drawPrimitives(graphics::PrimitiveType::triangle, 0, 3);
    }

    size_t frameValue = m_commandBufferAllocator->commit(moveCast(commandBuffer));
    m_frameQueue.pushBack(frameValue);
}

void Renderer::waitIdle()
{
    [[maybe_unsed]] apple::AutoreleasePool autoreleaesPool;

    // TODO
    SharedEvent sharedEvent(m_device);
    m_commandQueue->signal(addressof(sharedEvent), 1);
    sharedEvent.wait(1);
}

void Renderer::handleRenderError(PtrView<NS::Error> error)
{
    Renderer* renderer = interface->renderer();
    ASSERT(renderer);
    ENSURE(error == nullptr,
           error,
           "failed on event value {}. (signaled: {}, presented: {})",
           renderer->m_commandBufferAllocator->committedValue(),
           renderer->m_commandBufferAllocator->signaledValue(),
           renderer->m_frameValue) { }
}

} // namespace mini::metal4