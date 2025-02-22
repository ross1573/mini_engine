#include "mini_engine.h"

namespace mini::graphics
{

String ToString(API api)
{
    switch (api)
    {
        case API::D3D12: return String{"DirectX3D 12"};
        case API::Metal: return String{"Metal"};
        case API::Vulkan: return String{"Vulkan"};

        case API::Null:
        default: break;
    }

    return String{"null"};
}

} // namespace mini::graphics

namespace mini
{

bool Graphics::Initialize(graphics::Device* device)
{
    ASSERT(m_Device == nullptr && device != nullptr);

    m_Device = device;
    m_CurrentAPI = m_Device->GetAPI();
    ASSERT(m_CurrentAPI != graphics::API::Null);
    ASSERT(m_Device->Initialize(), "failed to initialize graphics device");

    m_RenderContext = m_Device->CreateRenderContext();
    ASSERT(m_RenderContext->Initialize(), "Failed to create render context");

    m_SwapChain = m_Device->CreateSwapChain();
    ASSERT(m_SwapChain->Initialize(), "Failed to create swap chain");

    return true;
}

void Graphics::Shutdown()
{
    DELETE(m_RenderContext);
    DELETE(m_SwapChain);
    DELETE(m_Device);

    m_CurrentAPI = API::Null;

#ifdef DEBUG
    if (DebugFunc != nullptr)
    {
        DebugFunc();
    }
#endif
}

void Graphics::BeginFrame()
{
    m_RenderContext->BeginRender();
}

void Graphics::EndFrame()
{
    m_RenderContext->EndRender();
    m_RenderContext->Execute();
    m_SwapChain->Present();
}

void Graphics::ChangeResolution(uint32 width, uint32 height, bool fullscreen)
{
    SwapChain* swapChain = GetSwapChain();
    if (swapChain == nullptr) [[unlikely]]
    {
        Log::Error("failed to change resolution. SwapChain not initialized");
        return;
    }

    swapChain->ResizeBackBuffer(width, height, fullscreen);
}

} // namespace mini