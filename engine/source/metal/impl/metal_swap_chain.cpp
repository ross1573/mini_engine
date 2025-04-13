module;

#include <Metal/MTLDevice.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#include "assertion.h"

module mini.metal;

import mini.core;
import mini.platform;
import mini.apple;
import :swap_chain;

namespace mini::metal {

SwapChain::SwapChain(MTL::Device* device)
    : m_Layer(nullptr)
    , m_Drawable(nullptr)
{
    m_Layer = CA::MetalLayer::layer();
    ASSERT(m_Layer, "failed to retrieve MetalLayer object");

    m_Layer->setDevice(device);
}

SwapChain::~SwapChain()
{
    if (m_Drawable) {
        m_Drawable->release();
    }

    if (m_Layer) {
        m_Layer->release();
    }
}

bool SwapChain::Initialize()
{
    auto window = (apple::Window*)Platform::GetWindow();
    window->SetMetalLayer(m_Layer);

    return true;
}

void SwapChain::Present()
{
    if (m_Drawable == nullptr) {
        return;
    }

    m_Drawable->release();
    m_Drawable = nullptr;
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

CA::MetalDrawable* SwapChain::GetCurrentDrawable()
{
    if (m_Drawable != nullptr) {
        return m_Drawable;
    }

    m_Drawable = m_Layer->nextDrawable();
    ASSERT(m_Drawable, "failed to retrieve next drawable");
    return m_Drawable;
}

} // namespace mini::metal