module;

#include <Metal/MTLDevice.hpp>

#include "assertion.h"

module mini.metal;

import mini.graphics;
import :swap_chain;
import :render_context;

namespace mini::metal {

Device::Device()
    : m_Device(nullptr)
{
}

Device::~Device()
{
    m_Device->release();
}

bool Device::Initialize()
{
    m_Device = MTL::CreateSystemDefaultDevice();
    ENSURE(m_Device, "Metal device not created") {
        return false;
    }

    return true;
}

graphics::SwapChain* Device::CreateSwapChain()
{
    return new SwapChain();
}

graphics::RenderContext* Device::CreateRenderContext()
{
    return new RenderContext();
}

} // namespace mini::metal