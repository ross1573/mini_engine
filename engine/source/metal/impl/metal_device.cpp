module;

#include <Metal/MTLDevice.hpp>

#include "assertion.h"

module mini.metal;

import mini.graphics;
import mini.apple;
import :swap_chain;
import :render_context;

namespace mini::metal {

Device::Device()
    : m_Device(nullptr)
{
}

bool Device::Initialize()
{
    m_Device = TransferShared(MTL::CreateSystemDefaultDevice());
    ENSURE(m_Device, "Metal device not created") {
        return false;
    }

    return true;
}

graphics::SwapChain* Device::CreateSwapChain()
{
    return new SwapChain(m_Device.Get());
}

graphics::RenderContext* Device::CreateRenderContext()
{
    return new RenderContext(m_Device.Get());
}

} // namespace mini::metal