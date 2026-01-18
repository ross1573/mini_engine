module mini.graphics;

import mini.core;
import :log;

namespace mini {

Graphics::Graphics() noexcept
{
    graphics::interface = this;
}

Graphics::~Graphics() noexcept
{
    m_RenderContext.Reset();
    m_SwapChain.Reset();
    m_Device.Reset();

    m_CurrentAPI = API::Null;
    graphics::interface = nullptr;
}

bool Graphics::Initialize()
{
    m_CurrentModule.Load(mini::options::graphicsModule);
    ENSURE(m_CurrentModule.Valid(), "failed to load graphics module") {
        return false;
    }

    String moduleName = m_CurrentModule.LibraryName();
    ENSURE(m_CurrentModule.GetInterface(), "module does not implement mini::graphics::Interface") {
        return false;
    }
    graphics::Log("{} module loaded", moduleName);

    Device* device = m_CurrentModule->CreateDevice();
    ENSURE(device, "failed to create graphic device") {
        return false;
    }
    graphics::Log("{} device created", moduleName);

    m_Device = UniquePtr(device);
    m_CurrentAPI = m_Device->GetAPI();

    ENSURE(m_CurrentAPI != API::Null, "unknown api") return false;
    ENSURE(m_Device->Initialize(), "failed to initialize graphics device") {
        return false;
    }
    graphics::Log("{} device initialized", m_CurrentAPI);

    m_RenderContext = UniquePtr(m_Device->CreateRenderContext());
    ENSURE(m_RenderContext && m_RenderContext->Initialize(), "Failed to create render context") {
        return false;
    }
    graphics::Log("{} render context initialized", m_CurrentAPI);

    m_SwapChain = UniquePtr(m_Device->CreateSwapChain());
    ENSURE(m_SwapChain && m_SwapChain->Initialize(), "Failed to create swap chain") {
        return false;
    }
    graphics::Log("{} swap chain initialized", m_CurrentAPI);

    return true;
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

bool Graphics::IsDeviceCurrent() noexcept
{
    return graphics::interface->GetDevice() != nullptr;
}

bool Graphics::IsDeviceCurrent(API api) noexcept
{
    return graphics::interface->GetDevice() != nullptr &&
           graphics::interface->GetCurrentAPI() == api;
}

void Graphics::ChangeResolution(uint32 width, uint32 height, bool fullscreen)
{
    SwapChain* swapChain = graphics::interface->GetSwapChain();
    if (swapChain == nullptr) [[unlikely]] {
        // graphics::LogError("failed to change resolution. SwapChain is not initialized");
        return;
    }

    swapChain->ResizeBackBuffer(width, height, fullscreen);
}

} // namespace mini