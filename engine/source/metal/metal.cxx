export module mini.metal;

export import :device;
export import :swap_chain;
export import :render_context;

import mini.graphics;

namespace mini::metal {

export class METAL_API Interface final : public Graphics {
public:
    Interface() noexcept;
    ~Interface() noexcept final;

    graphics::Device* CreateDevice() final { return new metal::Device(); }

    metal::SwapChain* GetSwapChain() noexcept;
    metal::RenderContext* GetRenderContext() noexcept;
};

METAL_API Interface* interface = nullptr;

Interface::Interface() noexcept
{
    interface = this;
}

Interface::~Interface() noexcept
{
    interface = nullptr;
}

metal::SwapChain* Interface::GetSwapChain() noexcept
{
    return static_cast<metal::SwapChain*>(Get()->GetSwapChain());
}

metal::RenderContext* Interface::GetRenderContext() noexcept
{
    return static_cast<metal::RenderContext*>(Get()->GetRenderContext());
}

} // namespace mini::metal