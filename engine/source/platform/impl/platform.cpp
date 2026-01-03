module mini.platform;

import :log;

namespace mini::platform {

Interface::Interface() noexcept
{
    interface = this;
}

Interface::~Interface() noexcept
{
    m_Window.Reset();
    m_Handle.Reset();

    interface = nullptr;
}

void Interface::PollEvents()
{
    if (m_Handle != nullptr) {
        m_Handle->PollEvents();
    }
}

bool Interface::Initialize()
{
    m_NativeModule.Load(options::platformModule);
    ENSURE(m_NativeModule.IsValid(), "failed to load platform module") return false;

    Platform* platformInterface = m_NativeModule.GetInterface<Platform>();
    ENSURE(platformInterface, "platform interface not implemented") return false;

    Handle* handle = platformInterface->CreateHandle();
    ENSURE(handle && handle->IsValid(), "failed to create platform handle") return false;

    m_Handle = UniquePtr(handle);
    Log("platform handle created");

    Window* window = platformInterface->CreateWindow();
    ENSURE(window && window->IsValid(), "failed to create window handle") return false;

    m_Window = UniquePtr(window);
    Log("platform window created");

    return true;
}

} // namespace mini::platform

namespace mini {

void Platform::AlertError(StringView const& msg)
{
    Handle* handle = platform::interface->GetHandle();
    if (handle != nullptr) {
        handle->AlertError(msg);
    }
}

platform::Interface* Platform::Get() const noexcept
{
    return platform::interface;
}

} // namespace mini