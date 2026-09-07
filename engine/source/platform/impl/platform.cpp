module mini.platform;

import :interface;
import :log;

using namespace mini::platform;

namespace mini {

Platform::Platform() noexcept
{
    platform::interface = this;
}

Platform::~Platform() noexcept
{
    m_window.Reset();
    m_handle.Reset();

    platform::interface = nullptr;
}

bool Platform::LoadModule()
{
    m_nativeModule.Load(MODULE_NATIVE);
    ENSURE(m_nativeModule.Valid(), "failed to load platform module") {
        return false;
    }

    m_handle = UniquePtr(m_nativeModule->CreateHandle());
    m_window = UniquePtr(m_nativeModule->CreateWindow());

    ENSURE(m_handle, "failed to create platform handle") return false;
    ENSURE(m_window, "failed to create window handle") return false;

    return true;
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