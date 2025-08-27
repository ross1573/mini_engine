export module mini.metal:device;

import mini.core;
import mini.graphics;
import mini.apple;

namespace mini::metal {

export class METAL_API Device final : public graphics::Device {
private:
    SharedPtr<MTL::Device> m_Device;

public:
    Device();

    bool Initialize() final;

    graphics::SwapChain* CreateSwapChain() final;
    graphics::RenderContext* CreateRenderContext() final;

    inline graphics::API GetAPI() const final { return graphics::API::Metal; }
    inline MTL::Device* GetMTLDevice() const { return m_Device.Get(); }
};

} // namespace mini::metal