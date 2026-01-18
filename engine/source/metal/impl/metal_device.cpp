module mini.metal;

import mini.graphics;
import mini.apple;
import :swap_chain;
import :render_context;

namespace mini::metal {

Device::Device()
    : m_device(TransferShared(MTL::CreateSystemDefaultDevice()))
{
}

bool Device::Initialize()
{
    ENSURE(m_device, "Metal device not created") {
        return false;
    }

    return true;
}

graphics::SwapChain* Device::CreateSwapChain()
{
    return new SwapChain(m_device.Get());
}

graphics::RenderContext* Device::CreateRenderContext()
{
    return new RenderContext(m_device.Get());
}

} // namespace mini::metal