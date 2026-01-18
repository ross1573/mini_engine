module mini.platform;

import :interface;
import :log;

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

void Platform::PollEvents()
{
    if (m_handle != nullptr) {
        m_handle->PollEvents();
    }
}

bool Platform::Initialize()
{
    m_nativeModule.Load(mini::options::platformModule);
    ENSURE(m_nativeModule.Valid(), "failed to load platform module") return false;

    Handle* handle = m_nativeModule->CreateHandle();
    ENSURE(handle && handle->Valid(), "failed to create platform handle") {
        return false;
    }
    m_handle = UniquePtr(handle);
    platform::Log("platform handle created");

    Window* window = m_nativeModule->CreateWindow();
    ENSURE(window && window->Valid(), "failed to create window handle") {
        return false;
    }
    m_window = UniquePtr(window);
    platform::Log("platform window created");

    return true;
}

void Platform::AlertError(StringView const& msg)
{
    Handle* handle = platform::interface->GetHandle();
    if (handle != nullptr) {
        handle->AlertError(msg);
    }
}

} // namespace mini