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
    m_renderer.reset();
    m_swapChain.reset();
    m_device.reset();

    m_currentAPI = Api::Null;
    graphics::interface = nullptr;
}

bool Graphics::loadModule(StringView moduleName)
{
    m_currentModule.load(moduleName);

    ENSURE(m_currentModule.valid(), "failed to load graphics module") return false;
    ENSURE(m_currentModule.interface(), "module does not implement mini::graphics::Interface") return false;

    logInfo("{} module loaded", moduleName);

    m_device = UniquePtr(m_currentModule->createDevice());
    ENSURE(m_device, "failed to create graphic device") return false;
    logInfo("{} device created", moduleName);

    m_currentAPI = m_device->api();
    m_renderer = UniquePtr(m_device->createRenderer());
    m_swapChain = UniquePtr(m_device->createSwapChain());

    m_renderer->prepare();

    ENSURE(m_renderer) return false;
    ENSURE(m_swapChain) return false;
    logInfo("{} initialized", moduleName);
    return true;
}

void Graphics::renderFrame()
{
    m_renderer->render();
    m_swapChain->present();
}

bool Graphics::current() noexcept
{
    return interface->device() != nullptr;
}

bool Graphics::current(Api api) noexcept
{
    return interface->device() != nullptr && interface->api() == api;
}

void Graphics::changeResolution(uint32 width, uint32 height, bool fullscreen)
{
    SwapChain* swapChain = interface->swapChain();
    if (swapChain == nullptr) [[unlikely]] {
        // graphics::logError("failed to change resolution. SwapChain is not initialized");
        return;
    }

    swapChain->resizeFrameBuffer(width, height, fullscreen);
}

} // namespace mini