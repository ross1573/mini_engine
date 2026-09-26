module mini.metal4;

import mini.core;
import mini.platform;
import mini.apple;
import :swap_chain;

namespace mini::metal4 {

SwapChain::SwapChain(Device* device)
    : m_layer(TransferShared(CA::MetalLayer::layer()))
{
    ASSERT(m_layer, "failed to retrieve MetalLayer object");

    m_layer->setDevice(device->MTLDevice());
    interface->GetWindow()->SetMetalLayer(m_layer.Get());
}

void SwapChain::Present()
{
    m_drawable.Present();
    m_drawable.Reset();
}

void SwapChain::ResizeBackBuffer([[maybe_unused]] uint32 width,
                                 [[maybe_unused]] uint32 height,
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

UniquePtr<Texture> SwapChain::FrameTexture()
{
    if (!m_drawable.Valid()) {
        SwapNextDrawable();
    }

    return MakeUnique<Texture>(Texture(m_drawable.MTLTexture()));
}

Drawable* SwapChain::Drawable()
{
    if (!m_drawable.Valid()) {
        SwapNextDrawable();
    }

    return memory::AddressOf(m_drawable);
}

void SwapChain::SwapNextDrawable()
{
    m_drawable.Reset(m_layer->nextDrawable());
    ASSERT(m_drawable, "failed to retrieve next drawable");
}

} // namespace mini::metal4