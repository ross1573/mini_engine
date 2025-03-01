export module mini.graphics;

import mini.core;
export import :common;
export import :device;
export import :render_context;
export import :swap_chain;

export namespace mini
{

class Graphics
{
    friend class Engine;

public:
    typedef graphics::API API;
    typedef graphics::Device Device;
    typedef graphics::SwapChain SwapChain;
    typedef graphics::RenderContext RenderContext;

private:
    static inline API m_CurrentAPI = API::Null;
    static inline Device* m_Device = nullptr;
    static inline SwapChain* m_SwapChain = nullptr;
    static inline RenderContext* m_RenderContext = nullptr;

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
    return m_Device != nullptr;
}

inline bool Graphics::IsDeviceCurrent(API api) noexcept
{
    return m_Device != nullptr && m_CurrentAPI == api;
}

template <typename T>
inline T* Graphics::GetDevice() noexcept
{
    return static_cast<T*>(m_Device);
}

template <typename T>
inline T* Graphics::GetSwapChain() noexcept
{
    return static_cast<T*>(m_SwapChain);
}

template <typename T>
inline T* Graphics::GetRenderContext() noexcept
{
    return static_cast<T*>(m_RenderContext);
}

} // namespcae mini