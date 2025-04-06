module mini.metal;

import mini.core;
import mini.platform;
import mini.macos;
import :swap_chain;

namespace mini::metal {

SwapChain::SwapChain()
    : m_Layer(nullptr)
{
}

bool SwapChain::Initialize()
{
    auto window = (macos::Window*)Platform::GetWindow();
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

void SwapChain::SetFullScreen([[maybe_unused]] bool fullscreen)
{
}

} // namespace mini::metal