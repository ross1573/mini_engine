module mini.metal;

import mini.core;
import mini.platform;
import mini.apple;
import :swap_chain;

namespace mini::metal {

SwapChain::SwapChain(MTL::Device* device)
    : m_layer(TransferShared(CA::MetalLayer::layer()))
    , m_drawable(nullptr)
{
    ASSERT(m_layer, "failed to retrieve MetalLayer object");

    m_layer->setDevice(device);
}

bool SwapChain::Initialize()
{
    interface->GetWindow()->SetMetalLayer(m_layer.Get());
    return true;
}

void SwapChain::Present()
{
    m_drawable.Reset();
}

void SwapChain::ResizeBackBuffer([[maybe_unused]] uint32 width, [[maybe_unused]] uint32 height,
                                 [[maybe_unused]] bool fullscreen)
{
}

void SwapChain::SetBackBufferCount([[maybe_unused]] uint8 count)
{
}

void SwapChain::SetVSync([[maybe_unused]] uint8 vsync)
{
}

void SwapChain::SetFullScreen(bool fullscreen)
{
    interface->GetWindow()->SetFullScreen(fullscreen);
}

bool SwapChain::GetFullScreen() const
{
    return interface->GetWindow()->IsFullScreen();
}

CA::MetalDrawable* SwapChain::GetCurrentDrawable()
{
    if (m_drawable != nullptr) {
        return m_drawable.Get();
    }

    m_drawable.Reset(m_layer->nextDrawable());
    ASSERT(m_drawable, "failed to retrieve next drawable");
    return m_drawable.Get();
}

} // namespace mini::metal