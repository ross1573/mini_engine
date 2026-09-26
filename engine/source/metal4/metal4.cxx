export module mini.metal4;

export import :common;
export import :buffer;
export import :texture;
export import :shader;
export import :command_buffer;
export import :command_queue;
export import :render_pass;
export import :render_pipeline;
export import :device;
export import :swap_chain;
export import :renderer;

import mini.platform;
import mini.graphics;

namespace mini {

export class METAL4_API Metal4 final : public graphics::Interface {
private:
    Module<Platform> m_platform;

public:
    Metal4() noexcept;
    ~Metal4() noexcept final;

    graphics::Device* CreateDevice() final { return new metal4::Device(); }

    apple::Window* GetWindow() noexcept;
    metal4::Device* GetDevice() noexcept;
    metal4::SwapChain* GetSwapChain() noexcept;
    metal4::Renderer* GetRenderer() noexcept;
};

} // namespace mini

namespace mini::metal4 {

METAL4_API Metal4* interface = nullptr;

} // namespace mini::metal4