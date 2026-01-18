module mini.metal;

namespace mini {

Metal::Metal() noexcept
    : m_platform("platform")
{
    ASSERT(m_platform.Valid());
    metal::interface = this;
}

Metal::~Metal() noexcept
{
    metal::interface = nullptr;
}

apple::Window* Metal::GetWindow() noexcept
{
    return static_cast<apple::Window*>(m_platform->GetWindow());
}

metal::Device* Metal::GetDevice() noexcept
{
    return static_cast<metal::Device*>(m_graphics->GetDevice());
}

metal::SwapChain* Metal::GetSwapChain() noexcept
{
    return static_cast<metal::SwapChain*>(m_graphics->GetSwapChain());
}

metal::RenderContext* Metal::GetRenderContext() noexcept
{
    return static_cast<metal::RenderContext*>(m_graphics->GetRenderContext());
}

} // namespace mini