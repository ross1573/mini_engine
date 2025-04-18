export module mini.graphics;

import mini.core;
export import :common;
export import :device;
export import :render_context;
export import :swap_chain;

namespace mini {

GRAPHICS_API graphics::API g_CurrAPI = graphics::API::Null;
GRAPHICS_API UniquePtr<graphics::Device> g_Device;
GRAPHICS_API UniquePtr<graphics::SwapChain> g_SwapChain;
GRAPHICS_API UniquePtr<graphics::RenderContext> g_RenderContext;

export class GRAPHICS_API Graphics {
public:
    typedef graphics::API API;
    typedef graphics::Device Device;
    typedef graphics::SwapChain SwapChain;
    typedef graphics::RenderContext RenderContext;

public:
    Graphics() = default;

    static bool Initialize(Device*);
    static void Shutdown();

    static void BeginFrame();
    static void EndFrame();
    static void ChangeResolution(uint32, uint32, bool);

    static bool IsDeviceCurrent() noexcept { return g_Device != nullptr; }
    static bool IsDeviceCurrent(API api) noexcept { return (bool)g_Device && g_CurrAPI == api; }

    inline static Device* GetDevice() noexcept { return g_Device.Get(); }
    inline static SwapChain* GetSwapChain() noexcept { return g_SwapChain.Get(); }
    inline static RenderContext* GetRenderContext() noexcept { return g_RenderContext.Get(); }
};

} // namespace mini