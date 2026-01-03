module mini.graphics;

import mini.core;
import :log;

namespace mini::graphics {

Interface::Interface() noexcept
{
    interface = this;
}

Interface::~Interface() noexcept
{
    m_RenderContext.Reset();
    m_SwapChain.Reset();
    m_Device.Reset();

    m_CurrentAPI = API::Null;
    interface = nullptr;
}

bool Interface::Initialize()
{
    m_CurrentModule.Load(options::graphicsModule);
    ENSURE(m_CurrentModule.IsValid(), "failed to load graphics module") {
        return false;
    }

    String moduleName = m_CurrentModule.GetName();
    Graphics* current = m_CurrentModule.GetInterface<Graphics>();
    ENSURE(current, "has not implemented mini::graphics::Interface") {
        return false;
    }
    Log(moduleName + " module loaded");

    Device* device = current->CreateDevice();
    ENSURE(device, "failed to create graphic device") {
        return false;
    }
    Log(moduleName + " device created");

    m_Device = UniquePtr(device);
    m_CurrentAPI = m_Device->GetAPI();

    ENSURE(m_CurrentAPI != API::Null, "unknown api") return false;
    ENSURE(m_Device->Initialize(), "failed to initialize graphics device") {
        return false;
    }
    Log("{} device initialized", m_CurrentAPI);

    m_RenderContext = UniquePtr(m_Device->CreateRenderContext());
    ENSURE(m_RenderContext && m_RenderContext->Initialize(), "Failed to create render context") {
        return false;
    }
    Log("{} render context initialized", m_CurrentAPI);

    m_SwapChain = UniquePtr(m_Device->CreateSwapChain());
    ENSURE(m_SwapChain && m_SwapChain->Initialize(), "Failed to create swap chain") {
        return false;
    }
    Log("{} swap chain initialized", m_CurrentAPI);

    return true;
}

void Interface::BeginFrame()
{
    m_RenderContext->BeginRender();
}

void Interface::EndFrame()
{
    m_RenderContext->EndRender();
    m_RenderContext->Execute();

    m_SwapChain->Present();
}

} // namespace mini::graphics

namespace mini {

void Graphics::ChangeResolution(uint32 width, uint32 height, bool fullscreen)
{
    SwapChain* swapChain = graphics::interface->GetSwapChain();
    if (swapChain == nullptr) [[unlikely]] {
        graphics::LogError("failed to change resolution. SwapChain is not initialized");
        return;
    }

    swapChain->ResizeBackBuffer(width, height, fullscreen);
}

} // namespace mini