module mini.metal4;

import mini.graphics;
import mini.apple;
import :swap_chain;
import :renderer;
import :buffer;

namespace mini::metal4 {

Device::Device()
    : m_device(TransferShared(CreateSystemDefaultDevice()))
{
}

graphics::SwapChain* Device::CreateSwapChain()
{
    return new SwapChain(m_device.Get());
}

graphics::Renderer* Device::CreateRenderer()
{
    return new Renderer(*this);
}

} // namespace mini::metal4