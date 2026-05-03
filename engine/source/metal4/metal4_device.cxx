export module mini.metal4:device;

import mini.graphics;
import mini.apple;

namespace mini::metal4 {

export class METAL4_API Device final : public graphics::Device {
private:
    SharedPtr<MTL::Device> m_device;

public:
    Device();

    bool Initialize() final;

    graphics::SwapChain* CreateSwapChain() final;
    graphics::Renderer* CreateRenderer() final;

    graphics::API GetAPI() const final { return graphics::API::Metal4; }
    MTL::Device* GetMTL4Device() const { return m_device.Get(); }
};

} // namespace mini::metal4