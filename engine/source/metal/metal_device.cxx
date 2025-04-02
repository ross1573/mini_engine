module;

#include <Metal/Metal.hpp>

export module mini.metal:device;

import mini.core;
import mini.graphics;
import mini.macos;

namespace mini::metal {

export class METAL_API Device final : public graphics::Device {
private:
    MTL::Device* m_Device;

public:
    Device();
    ~Device() final;

    bool Initialize() final;

    graphics::SwapChain* CreateSwapChain() final;
    graphics::RenderContext* CreateRenderContext() final;

    inline graphics::API GetAPI() const final { return graphics::API::Metal; }
    inline MTL::Device* GetMTLDevice() const { return m_Device; }
};

} // namespace mini::metal