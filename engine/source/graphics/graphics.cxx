export module mini.graphics;

import mini.core;

export import :common;
export import :device;
export import :render_context;
export import :swap_chain;

namespace mini::graphics {

export class GRAPHICS_API Interface final : public Module::Interface {
private:
    Module m_CurrentModule;
    API m_CurrentAPI;

    UniquePtr<Device> m_Device;
    UniquePtr<SwapChain> m_SwapChain;
    UniquePtr<RenderContext> m_RenderContext;

public:
    Interface() noexcept;
    ~Interface() noexcept final;

    void BeginFrame();
    void EndFrame();

    Module GetCurrentModule() const noexcept { return m_CurrentModule; }
    API GetCurrentAPI() const noexcept { return m_CurrentAPI; }

    Device* GetDevice() const noexcept { return m_Device.Get(); }
    SwapChain* GetSwapChain() const noexcept { return m_SwapChain.Get(); }
    RenderContext* GetRenderContext() const noexcept { return m_RenderContext.Get(); }

private:
    bool Initialize() final;
};

GRAPHICS_API Interface* interface = nullptr;

} // namespace mini::graphics

namespace mini {

export class GRAPHICS_API Graphics : public Module::Interface {
private:
    typedef graphics::Interface Interface;

public:
    typedef graphics::API API;
    typedef graphics::Device Device;
    typedef graphics::SwapChain SwapChain;
    typedef graphics::RenderContext RenderContext;

public:
    virtual Device* CreateDevice() = 0;

    static void ChangeResolution(uint32, uint32, bool);

    static bool IsDeviceCurrent() noexcept;
    static bool IsDeviceCurrent(API) noexcept;

protected:
    Interface* Get() const noexcept { return graphics::interface; }
};

bool Graphics::IsDeviceCurrent() noexcept
{
    return graphics::interface->GetDevice() != nullptr;
}

bool Graphics::IsDeviceCurrent(API api) noexcept
{
    return graphics::interface->GetDevice() != nullptr &&
           graphics::interface->GetCurrentAPI() == api;
}

} // namespace mini