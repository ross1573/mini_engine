export module mini.graphics;

import mini.core;
export import :common;
export import :device;
export import :render_context;
export import :swap_chain;

namespace mini
{

GRAPHICS_API graphics::API g_CurrentAPI = graphics::API::Null;
GRAPHICS_API UniquePtr<graphics::Device> g_Device;
GRAPHICS_API UniquePtr<graphics::SwapChain> g_SwapChain;
GRAPHICS_API UniquePtr<graphics::RenderContext> g_RenderContext;

export class GRAPHICS_API Graphics
{
    friend class Engine;

public:
    typedef graphics::API API;
    typedef graphics::Device Device;
    typedef graphics::SwapChain SwapChain;
    typedef graphics::RenderContext RenderContext;

public:
    static void ChangeResolution(uint32, uint32, bool);

    static bool IsDeviceCurrent() noexcept;
    static bool IsDeviceCurrent(API) noexcept;

    template <typename T = Device> static T* GetDevice() noexcept;
    template <typename T = SwapChain> static T* GetSwapChain() noexcept;
    template <typename T = RenderContext> static T* GetRenderContext() noexcept;

private:
    Graphics() = default;

    static bool Initialize(Device*);
    static void Shutdown();

    static void BeginFrame();
    static void EndFrame();
};

inline bool Graphics::IsDeviceCurrent() noexcept
{
    return g_Device != nullptr;
}

inline bool Graphics::IsDeviceCurrent(API api) noexcept
{
    return g_Device != nullptr && g_CurrentAPI == api;
}

template <typename T>
inline T* Graphics::GetDevice() noexcept
{
    return static_cast<T*>(g_Device.Get());
}

template <typename T>
inline T* Graphics::GetSwapChain() noexcept
{
    return static_cast<T*>(g_SwapChain.Get());
}

template <typename T>
inline T* Graphics::GetRenderContext() noexcept
{
    return static_cast<T*>(g_RenderContext.Get());
}

} // namespcae mini