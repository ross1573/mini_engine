module mini.platform;

import :interface;
import :log;

using namespace mini::platform;

namespace mini {

Platform::Platform() noexcept
    : m_nativeModule(MODULE_NATIVE)
{
    ENSURE(m_nativeModule.Valid(), "failed to load platform module") {
        return;
    }

    platform::interface = this;
    m_handle = UniquePtr(m_nativeModule->CreateHandle());
    m_window = UniquePtr(m_nativeModule->CreateWindow());

    ASSERT(m_handle, "failed to create platform handle");
    ASSERT(m_window, "failed to create window handle");
}

Platform::~Platform() noexcept
{
    m_window.Reset();
    m_handle.Reset();

    platform::interface = nullptr;
}

void Platform::PollEvents()
{
    if (m_handle != nullptr) {
        m_handle->PollEvents();
    }
}

void Platform::AlertError(StringView const& msg)
{
    Handle* handle = interface->GetHandle();
    if (handle != nullptr) {
        handle->AlertError(msg);
    }
}

} // namespace mini