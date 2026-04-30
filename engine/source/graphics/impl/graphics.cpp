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
    m_renderer.Reset();
    m_swapChain.Reset();
    m_device.Reset();

    m_currentAPI = API::Null;
    graphics::interface = nullptr;
}

bool Graphics::Initialize()
{
    m_currentModule.Load(mini::options::graphicsModule);
    ENSURE(m_currentModule.Valid(), "failed to load graphics module") {
        return false;
    }

    String moduleName = m_currentModule.LibraryName();
    ENSURE(m_currentModule.GetInterface(), "module does not implement mini::graphics::Interface") {
        return false;
    }
    graphics::Log("{} module loaded", moduleName);

    Device* device = m_currentModule->CreateDevice();
    ENSURE(device, "failed to create graphic device") {
        return false;
    }
    graphics::Log("{} device created", moduleName);

    m_device = UniquePtr(device);
    m_currentAPI = m_device->GetAPI();

    ENSURE(m_currentAPI != API::Null, "unknown api") return false;
    ENSURE(m_device->Initialize(), "failed to initialize graphics device") {
        return false;
    }
    graphics::Log("{} device initialized", m_currentAPI);

    m_renderer = UniquePtr(m_device->CreateRenderer());
    ENSURE(m_renderer && m_renderer->Initialize(), "Failed to create render context") {
        return false;
    }
    graphics::Log("{} render context initialized", m_currentAPI);

    m_swapChain = UniquePtr(m_device->CreateSwapChain());
    ENSURE(m_swapChain && m_swapChain->Initialize(), "Failed to create swap chain") {
        return false;
    }
    graphics::Log("{} swap chain initialized", m_currentAPI);

    return true;
}

void Graphics::BeginFrame()
{
    m_renderer->BeginRender();
}

void Graphics::EndFrame()
{
    m_renderer->EndRender();
    m_renderer->Execute();

    m_swapChain->Present();
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