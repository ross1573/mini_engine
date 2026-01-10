export module mini.graphics;

import mini.core;

export import :common;
export import :interface;
export import :device;
export import :render_context;
export import :swap_chain;

namespace mini {

export class GRAPHICS_API Graphics final : public ModuleInterface {
public:
    typedef graphics::API API;
    typedef graphics::Interface Interface;

    typedef graphics::Device Device;
    typedef graphics::SwapChain SwapChain;
    typedef graphics::RenderContext RenderContext;

private:
    API m_CurrentAPI;
    Module<Interface> m_CurrentModule;

    UniquePtr<Device> m_Device;
    UniquePtr<SwapChain> m_SwapChain;
    UniquePtr<RenderContext> m_RenderContext;

public:
    Graphics() noexcept;
    ~Graphics() noexcept final;

    void BeginFrame();
    void EndFrame();

    API GetCurrentAPI() const noexcept { return m_CurrentAPI; }
    Module<Interface> GetCurrentModule() const noexcept { return m_CurrentModule; }

    Device* GetDevice() const noexcept { return m_Device.Get(); }
    SwapChain* GetSwapChain() const noexcept { return m_SwapChain.Get(); }
    RenderContext* GetRenderContext() const noexcept { return m_RenderContext.Get(); }

    static void ChangeResolution(uint32, uint32, bool);

    static bool IsDeviceCurrent() noexcept;
    static bool IsDeviceCurrent(API) noexcept;

private:
    bool Initialize() final;
};

} // namespace mini