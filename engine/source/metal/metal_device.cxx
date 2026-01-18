export module mini.metal:device;

import mini.graphics;
import mini.apple;

namespace mini::metal {

export class METAL_API Device final : public graphics::Device {
private:
    SharedPtr<MTL::Device> m_device;

public:
    Device();

    bool Initialize() final;

    graphics::SwapChain* CreateSwapChain() final;
    graphics::RenderContext* CreateRenderContext() final;

    graphics::API GetAPI() const final { return graphics::API::Metal; }
    MTL::Device* GetMTLDevice() const { return m_device.Get(); }
};

} // namespace mini::metal