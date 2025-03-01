module;

#include "core/macro.h"
#include "core/assert.h"

module mini.graphics;

import mini.core;

namespace mini
{

bool Graphics::Initialize(graphics::Device* device)
{
    VERIFY(m_Device == nullptr && device != nullptr);

    m_Device = device;
    m_CurrentAPI = m_Device->GetAPI();
    VERIFY(m_CurrentAPI != graphics::API::Null);
    VERIFY(m_Device->Initialize(), "failed to initialize graphics device");

    m_RenderContext = m_Device->CreateRenderContext();
    VERIFY(m_RenderContext->Initialize(), "Failed to create render context");

    m_SwapChain = m_Device->CreateSwapChain();
    VERIFY(m_SwapChain->Initialize(), "Failed to create swap chain");

    return true;
}

void Graphics::Shutdown()
{
    DELETE(m_RenderContext);
    DELETE(m_SwapChain);
    DELETE(m_Device);

    m_CurrentAPI = API::Null;
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
        Log::Error("failed to change resolution. SwapChain is not initialized");
        return;
    }

    swapChain->ResizeBackBuffer(width, height, fullscreen);
}

} // namespace mini