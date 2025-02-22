#pragma once

#include "core/utility.h"
#include "container/string.h"

namespace mini::graphics
{

class Device;
class SwapChain;
class RenderContext;

enum class API : byte
{
    Null = 0,

    D3D12,
    Metal,
    Vulkan,
};

enum class CommandType : int8
{
    None = -1,

    Direct,
    Compute,
    Copy
};

String ToString(API);

} // namespace mini:::graphics

namespace mini
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

private:
    Graphics() = default;

    static bool Initialize(Device*);
    static void Shutdown();

    static void BeginFrame();
    static void EndFrame();

public:
    static void ChangeResolution(uint32, uint32, bool);

    static inline bool IsDeviceCurrent() noexcept;
    static inline bool IsDeviceCurrent(API) noexcept;

    template <typename T = Device> static T* GetDevice() noexcept;
    template <typename T = SwapChain> static T* GetSwapChain() noexcept;
    template <typename T = RenderContext> static T* GetRenderContext() noexcept;

#ifdef DEBUG
    typedef void (*DebugFuncT)();
    static inline DebugFuncT DebugFunc = nullptr;
#endif
};

[[force_inline]] bool Graphics::IsDeviceCurrent() noexcept
{
    return m_Device != nullptr;
}

[[force_inline]] bool Graphics::IsDeviceCurrent(API api) noexcept
{
    return m_Device != nullptr && m_CurrentAPI == api;
}

template <typename T>
[[force_inline]] T* Graphics::GetDevice() noexcept
{
    return static_cast<T*>(m_Device);
}

template <typename T>
[[force_inline]] T* Graphics::GetSwapChain() noexcept
{
    return static_cast<T*>(m_SwapChain);
}

template <typename T>
[[force_inline]] T* Graphics::GetRenderContext() noexcept
{
    return static_cast<T*>(m_RenderContext);
}

} // namespcae mini