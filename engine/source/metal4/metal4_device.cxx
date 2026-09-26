module;

#include <Metal/MTLDevice.hpp>

export module mini.metal4:device;

import mini.graphics;
import mini.apple;

export namespace MTL {

using MTL::Device;

} // namespace MTL

namespace mini::metal4 {

export class METAL4_API Device final : public graphics::Device {
private:
    SharedPtr<MTL::Device> m_device;

public:
    Device();

    [[nodiscard]] bool Valid() const noexcept final { return m_device.Valid(); }

    [[nodiscard]] graphics::SwapChain* CreateSwapChain() final;
    [[nodiscard]] graphics::Renderer* CreateRenderer() final;

    [[nodiscard]] graphics::API GetAPI() const noexcept final { return graphics::API::Metal4; }
    [[nodiscard]] MTL::Device* MTLDevice() const noexcept { return m_device.Get(); }
};

MTL::Device* CreateSystemDefaultDevice() noexcept
{
    return MTL::CreateSystemDefaultDevice();
}

} // namespace mini::metal4