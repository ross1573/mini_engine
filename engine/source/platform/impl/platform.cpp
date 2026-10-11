module mini.platform;

import :interface;
import :log;

using namespace mini::platform;

namespace mini {

Platform::Platform() noexcept
{
    platform::interface = this;

    m_nativeModule.load(MODULE_NATIVE);
    m_handle = UniquePtr(m_nativeModule->createHandle());
    m_window = UniquePtr(m_nativeModule->createWindow());

    ASSERT(m_nativeModule.valid(), "failed to load platform module");
    ASSERT(m_handle, "failed to create platform handle");
    ASSERT(m_window, "failed to create window handle");
}

Platform::~Platform() noexcept
{
    m_window.reset();
    m_handle.reset();

    platform::interface = nullptr;
}

void Platform::pollEvents()
{
    if (m_handle != nullptr) {
        m_handle->pollEvents();
    }
}

void Platform::alertError(StringView const& msg)
{
    Handle* handle = interface->handle();
    if (handle != nullptr) {
        handle->alertError(msg);
    }
}

} // namespace mini