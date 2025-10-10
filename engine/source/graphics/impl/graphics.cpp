module mini.graphics;

import mini.core;
import :log;

namespace mini {

bool Graphics::Initialize()
{
    ASSERT(g_Device == nullptr);

    return LoadAPI(mini::options::graphicsAPI);
}

bool Graphics::LoadAPI(StringView api)
{
    Module graphicsModule = Module(api);
    ENSURE(graphicsModule.IsValid(), "failed to load graphics module") {
        return false;
    }
    graphics::Log("{0} module loaded", api);

    using GraphicsInterface = mini::graphics::ModuleInterface;
    GraphicsInterface* interface = graphicsModule.GetInterface<GraphicsInterface>();
    ENSURE(interface,
           Format("{0} graphics has not been implemented mini::graphics::ModuleInterface",
                  mini::options::graphicsAPI)
               .Data()) {
        return false;
    }

    using GraphicsDevice = mini::graphics::Device;
    GraphicsDevice* device = interface->CreateGraphicDevice();
    ENSURE(device, "failed to create graphic device") {
        return false;
    }
    graphics::Log("{0} device created", api);

    g_Graphics = MakeUnique<Graphics>();
    g_Graphics->m_Module = MoveArg(graphicsModule);
    g_Device = UniquePtr(device);
    g_CurrAPI = g_Device->GetAPI();

    ENSURE(g_CurrAPI != API::Null, "unknown api") return false;
    ENSURE(g_Device->Initialize(), "failed to initialize graphics device") {
        return false;
    }
    graphics::Log("{0} device initialized", api);

    g_RenderContext = UniquePtr(g_Device->CreateRenderContext());
    ENSURE(g_RenderContext && g_RenderContext->Initialize(), "Failed to create render context") {
        return false;
    }
    graphics::Log("render context initialized");

    g_SwapChain = UniquePtr(g_Device->CreateSwapChain());
    ENSURE(g_SwapChain && g_SwapChain->Initialize(), "Failed to create swap chain") {
        return false;
    }
    graphics::Log("swap chain initialized");

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
        graphics::LogError("failed to change resolution. SwapChain is not initialized");
        return;
    }

    swapChain->ResizeBackBuffer(width, height, fullscreen);
}

} // namespace mini