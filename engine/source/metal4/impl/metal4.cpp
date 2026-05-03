module mini.metal4;

namespace mini {

Metal4::Metal4() noexcept
    : m_platform("platform")
{
    ASSERT(m_platform.Valid());
    metal4::interface = this;
}

Metal4::~Metal4() noexcept
{
    metal4::interface = nullptr;
}

apple::Window* Metal4::GetWindow() noexcept
{
    return static_cast<apple::Window*>(m_platform->GetWindow());
}

metal4::Device* Metal4::GetDevice() noexcept
{
    return static_cast<metal4::Device*>(m_graphics->GetDevice());
}

metal4::SwapChain* Metal4::GetSwapChain() noexcept
{
    return static_cast<metal4::SwapChain*>(m_graphics->GetSwapChain());
}

metal4::Renderer* Metal4::GetRenderer() noexcept
{
    return static_cast<metal4::Renderer*>(m_graphics->GetRenderer());
}

} // namespace mini