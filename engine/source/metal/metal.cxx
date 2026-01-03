export module mini.metal;

export import :device;
export import :swap_chain;
export import :render_context;

import mini.platform;
import mini.graphics;

namespace mini::metal {

export class METAL_API Interface final : public Graphics {
private:
    Module m_PlatformModule;
    platform::Interface* m_Platform;
    graphics::Interface* m_Graphics;

public:
    Interface() noexcept;
    ~Interface() noexcept final;

    graphics::Device* CreateDevice() final { return new metal::Device(); }

    apple::Window* GetWindow() noexcept;

    metal::SwapChain* GetSwapChain() noexcept;
    metal::RenderContext* GetRenderContext() noexcept;

private:
    bool Initialize() final;
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

bool Interface::Initialize()
{
    m_Graphics = Get();

    m_PlatformModule.Load("platform");
    ENSURE(m_PlatformModule.IsValid()) {
        return false;
    }

    m_Platform = m_PlatformModule.GetInterface<platform::Interface>();
    return m_Platform != nullptr && m_Graphics != nullptr;
}

apple::Window* Interface::GetWindow() noexcept
{
    return static_cast<apple::Window*>(m_Platform->GetWindow());
}

metal::SwapChain* Interface::GetSwapChain() noexcept
{
    return static_cast<metal::SwapChain*>(m_Graphics->GetSwapChain());
}

metal::RenderContext* Interface::GetRenderContext() noexcept
{
    return static_cast<metal::RenderContext*>(m_Graphics->GetRenderContext());
}

} // namespace mini::metal