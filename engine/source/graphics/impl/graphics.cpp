module;

#include "assertion.h"

module mini.graphics;

import mini.core;

namespace mini {

bool Graphics::Initialize(Device* device)
{
    VERIFY(g_Device == nullptr && device != nullptr);

    g_Device = UniquePtr(device);
    g_CurrAPI = g_Device->GetAPI();
    VERIFY(g_CurrAPI != API::Null);
    VERIFY(g_Device->Initialize(), "failed to initialize graphics device");

    g_RenderContext = UniquePtr(g_Device->CreateRenderContext());
    VERIFY(g_RenderContext->Initialize(), "Failed to create render context");

    g_SwapChain = UniquePtr(g_Device->CreateSwapChain());
    VERIFY(g_SwapChain->Initialize(), "Failed to create swap chain");

    return true;
}

void Graphics::Shutdown()
{
    g_RenderContext.Reset();
    g_SwapChain.Reset();
    g_Device.Reset();
    g_CurrAPI = API::Null;
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
    if (swapChain == nullptr) [[unlikely]] {
        log::Error("failed to change resolution. SwapChain is not initialized");
        return;
    }

    swapChain->ResizeBackBuffer(width, height, fullscreen);
}

} // namespace mini