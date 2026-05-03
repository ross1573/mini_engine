module mini.metal4;

import mini.graphics;
import mini.apple;
import :swap_chain;
import :renderer;

namespace mini::metal4 {

Device::Device()
    : m_device(TransferShared(MTL::CreateSystemDefaultDevice()))
{
}

bool Device::Initialize()
{
    ENSURE(m_device, "Metal4 device not created") {
        return false;
    }

    return true;
}

graphics::SwapChain* Device::CreateSwapChain()
{
    return new SwapChain(m_device.Get());
}

graphics::Renderer* Device::CreateRenderer()
{
    return new Renderer(m_device.Get());
}

} // namespace mini::metal4