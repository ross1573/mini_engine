export module mini.graphics;

import mini.core;
export import :common;
export import :device;
export import :render_context;
export import :swap_chain;

GRAPHICS_API mini::graphics::API g_CurrentAPI = mini::graphics::API::Null;
GRAPHICS_API mini::graphics::Device* g_Device = nullptr;
GRAPHICS_API mini::graphics::SwapChain* g_SwapChain = nullptr;
GRAPHICS_API mini::graphics::RenderContext* g_RenderContext = nullptr;

export namespace mini
{

class GRAPHICS_API Graphics
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

using namespace graphics;

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
    return static_cast<T*>(g_Device);
}

template <typename T>
inline T* Graphics::GetSwapChain() noexcept
{
    return static_cast<T*>(g_SwapChain);
}

template <typename T>
inline T* Graphics::GetRenderContext() noexcept
{
    return static_cast<T*>(g_RenderContext);
}

} // namespcae mini