module mini.graphics;

import mini.core;
import :log;

using namespace mini::graphics;

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

bool Graphics::LoadModule(StringView moduleName)
{
    m_currentModule.Load(moduleName);

    ENSURE(m_currentModule.Valid(), "failed to load graphics module") return false;
    ENSURE(m_currentModule.GetInterface(), "module does not implement mini::graphics::Interface") return false;

    LogInfo("{} module loaded", moduleName);

    m_device = UniquePtr(m_currentModule->CreateDevice());
    ENSURE(m_device, "failed to create graphic device") return false;
    LogInfo("{} device created", moduleName);

    m_currentAPI = m_device->GetAPI();
    m_renderer = UniquePtr(m_device->CreateRenderer());
    m_swapChain = UniquePtr(m_device->CreateSwapChain());

    ENSURE(m_renderer) return false;
    ENSURE(m_swapChain) return false;
    LogInfo("{} initialized", moduleName);
    return true;
}

void Graphics::RenderFrame()
{
    m_renderer->Render();
    m_renderer->Execute();
    m_swapChain->Present();
}

bool Graphics::IsDeviceCurrent() noexcept
{
    return interface->GetDevice() != nullptr;
}

bool Graphics::IsDeviceCurrent(API api) noexcept
{
    return interface->GetDevice() != nullptr && interface->CurrentAPI() == api;
}

void Graphics::ChangeResolution(uint32 width, uint32 height, bool fullscreen)
{
    SwapChain* swapChain = interface->GetSwapChain();
    if (swapChain == nullptr) [[unlikely]] {
        // graphics::LogError("failed to change resolution. SwapChain is not initialized");
        return;
    }

    swapChain->ResizeBackBuffer(width, height, fullscreen);
}

} // namespace mini