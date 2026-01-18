module mini.metal;

namespace mini {

Metal::Metal() noexcept
    : m_Platform("platform")
{
    ASSERT(m_Platform.Valid());
    metal::interface = this;
}

Metal::~Metal() noexcept
{
    metal::interface = nullptr;
}

apple::Window* Metal::GetWindow() noexcept
{
    return static_cast<apple::Window*>(m_Platform->GetWindow());
}

metal::Device* Metal::GetDevice() noexcept
{
    return static_cast<metal::Device*>(m_Graphics->GetDevice());
}

metal::SwapChain* Metal::GetSwapChain() noexcept
{
    return static_cast<metal::SwapChain*>(m_Graphics->GetSwapChain());
}

metal::RenderContext* Metal::GetRenderContext() noexcept
{
    return static_cast<metal::RenderContext*>(m_Graphics->GetRenderContext());
}

} // namespace mini