module mini.metal;

import mini.core;
import mini.platform;
import mini.apple;
import :swap_chain;

namespace mini::metal {

SwapChain::SwapChain()
{
}

bool SwapChain::Initialize()
{
    auto window = (apple::Window*)Platform::GetWindow();
    window->SetMetalLayer(m_Layer);

    return true;
}

void SwapChain::Present()
{
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
    static_cast<apple::Window*>(Platform::GetWindow())->SetFullScreen(fullscreen);
}

bool SwapChain::GetFullScreen() const
{
    return static_cast<apple::Window*>(Platform::GetWindow())->IsFullScreen();
}

} // namespace mini::metal