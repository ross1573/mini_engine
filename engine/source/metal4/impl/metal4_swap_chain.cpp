module mini.metal4;

import :swap_chain;

namespace mini::metal4 {

SwapChain::SwapChain(PtrView<Device> device)
    : m_layer(transfer(CA::MetalLayer::layer()))
{
    ASSERT(m_layer, "failed to retrieve MetalLayer object");
    ASSERT(options::bufferCount > 0 && options::bufferCount < 4, "invalid frame buffer count");

    m_layer->setDevice(device->mtlDevice());
    m_layer->setFramebufferOnly(true);
    m_layer->setDisplaySyncEnabled(options::vsyncRate > 0);
    m_layer->setMaximumDrawableCount(max(options::bufferCount, byte{2}));

    interface->window()->setMetalLayer(m_layer.get());
}

void SwapChain::present()
{
    m_drawable.present();
    m_drawable.reset();
}

void SwapChain::resizeFrameBuffer([[maybe_unused]] uint32 width,
                                  [[maybe_unused]] uint32 height,
                                  [[maybe_unused]] bool fullscreen)
{
}

void SwapChain::resizeBufferCount([[maybe_unused]] uint8 count)
{
}

void SwapChain::setVSync([[maybe_unused]] uint8 setVSync)
{
}

void SwapChain::setFullScreen(bool fullscreen)
{
    interface->window()->setFullScreen(fullscreen);
}

bool SwapChain::fullScreen() const
{
    return interface->window()->fullScreen();
}

Drawable* SwapChain::drawable()
{
    if (!m_drawable.valid()) {
        m_drawable.reset(m_layer->nextDrawable());
        ASSERT(m_drawable, "failed to retrieve next drawable");
    }

    return addressof(m_drawable);
}

} // namespace mini::metal4