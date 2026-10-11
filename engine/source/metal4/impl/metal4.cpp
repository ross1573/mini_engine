module mini.metal4;

namespace mini {

Metal4::Metal4() noexcept
    : m_platform("mini.platform")
{
    ASSERT(m_platform.valid());
    metal4::interface = this;
}

Metal4::~Metal4() noexcept
{
    metal4::interface = nullptr;
}

apple::Window* Metal4::window() noexcept
{
    return static_cast<apple::Window*>(m_platform->window());
}

metal4::Device* Metal4::device() noexcept
{
    return static_cast<metal4::Device*>(m_graphics->device());
}

metal4::SwapChain* Metal4::swapChain() noexcept
{
    return static_cast<metal4::SwapChain*>(m_graphics->swapChain());
}

metal4::Renderer* Metal4::renderer() noexcept
{
    return static_cast<metal4::Renderer*>(m_graphics->renderer());
}

} // namespace mini