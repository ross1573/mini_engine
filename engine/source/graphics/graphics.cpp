module;

#include "assertion.h"

module mini.graphics;

import mini.core;

namespace mini
{

bool Graphics::Initialize(graphics::Device* device)
{
    VERIFY(g_Device == nullptr && device != nullptr);

    g_Device = device;
    g_CurrentAPI = g_Device->GetAPI();
    VERIFY(g_CurrentAPI != graphics::API::Null);
    VERIFY(g_Device->Initialize(), "failed to initialize graphics device");

    g_RenderContext = g_Device->CreateRenderContext();
    VERIFY(g_RenderContext->Initialize(), "Failed to create render context");

    g_SwapChain = g_Device->CreateSwapChain();
    VERIFY(g_SwapChain->Initialize(), "Failed to create swap chain");

    return true;
}

void Graphics::Shutdown()
{
    DELETE(g_RenderContext);
    DELETE(g_SwapChain);
    DELETE(g_Device);

    g_CurrentAPI = API::Null;
}

void Graphics::BeginFrame()
{
    g_RenderContext->BeginRender();
}

void Graphics::EndFrame()
{
    g_RenderContext->EndRender();
    g_RenderContext->Execute();
    g_SwapChain->Present();
}

void Graphics::ChangeResolution(uint32 width, uint32 height, bool fullscreen)
{
    SwapChain* swapChain = GetSwapChain();
    if (swapChain == nullptr) [[unlikely]]
    {
        Log::Error("failed to change resolution. SwapChain is not initialized");
        return;
    }

    swapChain->ResizeBackBuffer(width, height, fullscreen);
}

} // namespace mini