module;

#include <Metal/MTLDevice.hpp>

#include "assertion.h"

module mini.metal;

import mini.graphics;

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
    ENSURE(m_Device, "Metal device not created") return false;

    return false;
}

graphics::SwapChain* Device::CreateSwapChain()
{
    return nullptr;
}

graphics::RenderContext* Device::CreateRenderContext()
{
    return nullptr;
}

} // namespace mini::metal