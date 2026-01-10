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
    m_Window.Reset();
    m_Handle.Reset();

    platform::interface = nullptr;
}

void Platform::PollEvents()
{
    if (m_Handle != nullptr) {
        m_Handle->PollEvents();
    }
}

bool Platform::Initialize()
{
    m_NativeModule.Load(mini::options::platformModule);
    ENSURE(m_NativeModule.IsValid(), "failed to load platform module") return false;

    Handle* handle = m_NativeModule->CreateHandle();
    ENSURE(handle && handle->IsValid(), "failed to create platform handle") {
        return false;
    }
    m_Handle = UniquePtr(handle);
    platform::Log("platform handle created");

    Window* window = m_NativeModule->CreateWindow();
    ENSURE(window && window->IsValid(), "failed to create window handle") {
        return false;
    }
    m_Window = UniquePtr(window);
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