export module mini.graphics;

import mini.core;

export import :common;
export import :interface;
export import :device;
export import :renderer;
export import :swap_chain;

namespace mini {

export class GRAPHICS_API Graphics final : public ModuleInterface {
public:
    typedef graphics::API API;
    typedef graphics::Interface Interface;

    typedef graphics::Device Device;
    typedef graphics::SwapChain SwapChain;
    typedef graphics::Renderer Renderer;

private:
    API m_currentAPI;
    Module<Interface> m_currentModule;

    UniquePtr<Device> m_device;
    UniquePtr<SwapChain> m_swapChain;
    UniquePtr<Renderer> m_renderer;

public:
    Graphics() noexcept;
    ~Graphics() noexcept;

    void BeginFrame();
    void EndFrame();

    API GetCurrentAPI() const noexcept { return m_currentAPI; }
    Module<Interface> GetCurrentModule() const noexcept { return m_currentModule; }

    Device* GetDevice() const noexcept { return m_device.Get(); }
    SwapChain* GetSwapChain() const noexcept { return m_swapChain.Get(); }
    Renderer* GetRenderer() const noexcept { return m_renderer.Get(); }

    static void ChangeResolution(uint32, uint32, bool);

    static bool IsDeviceCurrent() noexcept;
    static bool IsDeviceCurrent(API) noexcept;

private:
    bool Initialize() final;
};

} // namespace mini