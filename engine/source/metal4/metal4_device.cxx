export module mini.metal4:device;

import mini.graphics;
import mini.apple;

namespace mini::metal4 {

export class METAL4_API Device final : public graphics::Device {
private:
    SharedPtr<MTL::Device> m_device;

public:
    Device();

    [[nodiscard]] bool Valid() const noexcept final { return m_device.Valid(); }

    [[nodiscard]] graphics::SwapChain* CreateSwapChain() final;
    [[nodiscard]] graphics::Renderer* CreateRenderer() final;

    [[nodiscard]] graphics::API GetAPI() const final { return graphics::API::Metal4; }
    [[nodiscard]] MTL::Device* MTLDevice() const { return m_device.Get(); }
    [[nodiscard]] MTL::Device* operator->() const noexcept { return m_device.operator->(); }
};

} // namespace mini::metal4