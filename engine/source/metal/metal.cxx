export module mini.metal;

export import :device;
export import :swap_chain;
export import :render_context;

import mini.platform;
import mini.graphics;

namespace mini {

export class METAL_API Metal final : public graphics::Interface {
private:
    Module<Platform> m_Platform;

public:
    Metal() noexcept;
    ~Metal() noexcept;

    graphics::Device* CreateDevice() final { return new metal::Device(); }

    apple::Window* GetWindow() noexcept;

    metal::SwapChain* GetSwapChain() noexcept;
    metal::RenderContext* GetRenderContext() noexcept;
};

} // namespace mini

namespace mini::metal {

METAL_API Metal* interface = nullptr;

} // namespace mini::metal