module mini.metal4;

import :swap_chain;
import :renderer;
import :buffer;

namespace mini::metal4 {

Device::Device()
    : m_device(transfer(createSystemDefaultDevice()))
{
}

graphics::SwapChain* Device::createSwapChain()
{
    return new SwapChain(this);
}

graphics::Renderer* Device::createRenderer()
{
    return new Renderer(this);
}

} // namespace mini::metal4