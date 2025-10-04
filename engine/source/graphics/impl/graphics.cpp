module mini.graphics;

import mini.core;

namespace mini {

bool Graphics::Initialize(Device* device)
{
    ASSERT(g_Device == nullptr && device != nullptr);

    g_Graphics = MakeUnique<Graphics>();
    g_Device = UniquePtr(device);
    g_CurrAPI = g_Device->GetAPI();

    ENSURE(g_CurrAPI != API::Null, "unknown api") return false;
    ENSURE(g_Device->Initialize(), "failed to initialize graphics device") {
        return false;
    }
    log::Info("graphic device initialized");

    g_RenderContext = UniquePtr(g_Device->CreateRenderContext());
    ENSURE(g_RenderContext && g_RenderContext->Initialize(), "Failed to create render context") {
        return false;
    }
    log::Info("render context initialized");

    g_SwapChain = UniquePtr(g_Device->CreateSwapChain());
    ENSURE(g_SwapChain && g_SwapChain->Initialize(), "Failed to create swap chain") {
        return false;
    }
    log::Info("swap chain initialized");

    return true;
}

void Graphics::Shutdown()
{
    g_RenderContext.Reset();
    g_SwapChain.Reset();
    g_Device.Reset();
    g_CurrAPI = API::Null;

    for (auto& func : g_Graphics->m_ExitCallback) {
        func();
    }

    g_Graphics.Reset();
}

void Graphics::AtExit(CallbackFunc func)
{
    g_Graphics->m_ExitCallback.Push(func);
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