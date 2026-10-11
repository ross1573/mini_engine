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

    [[nodiscard]] bool valid() const noexcept final { return m_device.valid(); }

    [[nodiscard]] graphics::SwapChain* createSwapChain() final;
    [[nodiscard]] graphics::Renderer* createRenderer() final;

    [[nodiscard]] graphics::Api api() const noexcept final { return graphics::Api::Metal4; }
    [[nodiscard]] MTL::Device* mtlDevice() const noexcept { return m_device.get(); }
};

MTL::Device* createSystemDefaultDevice() noexcept
{
    return MTL::CreateSystemDefaultDevice();
}

} // namespace mini::metal4