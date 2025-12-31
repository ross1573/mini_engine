module mini.metal;

import mini.core;
import mini.graphics;
import mini.apple;
import :render_context;
import :swap_chain;

namespace mini::metal {

RenderContext::RenderContext(MTL::Device* device)
    : m_AutoReleasePool(nullptr)
    , m_CmdQueue(nullptr)
    , m_CmdBuffer(nullptr)
    , m_Event(nullptr)
    , m_EventValue(0)
{
    ASSERT(device);
    m_CmdQueue = TransferShared(device->newCommandQueue());
    m_Event = TransferShared(device->newSharedEvent());
}

bool RenderContext::Initialize()
{
    ENSURE(m_CmdQueue, "failed to create MTLCommandQueue") {
        return false;
    }

    return true;
}

void RenderContext::BeginRender()
{
    m_Event->waitUntilSignaledValue(m_EventValue, ~uint64(0));

    m_AutoReleasePool = TransferShared(NS::AutoreleasePool::alloc()->init());
    m_CmdBuffer = m_CmdQueue->commandBuffer();
    m_Drawable = ((SwapChain*)Graphics::GetSwapChain())->GetCurrentDrawable();

    MTL::Texture* texture = m_Drawable->texture();
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

    m_CmdEncoder = m_CmdBuffer->renderCommandEncoder(renderPass);
}

void RenderContext::EndRender()
{
}

void RenderContext::WaitForIdle()
{
}

void RenderContext::Execute()
{
    m_CmdEncoder->endEncoding();
    m_CmdBuffer->presentDrawable(m_Drawable);

    m_EventValue++;
    m_CmdBuffer->encodeSignalEvent(m_Event.Get(), m_EventValue);
    m_CmdBuffer->commit();

    m_Drawable = nullptr;
    m_CmdEncoder = nullptr;
    m_CmdBuffer = nullptr;
    m_AutoReleasePool.Reset();
}

void RenderContext::SetViewport(Rect const&, float32, float32)
{
}

void RenderContext::SetScissorRect(RectInt const&)
{
}

} // namespace mini::metal