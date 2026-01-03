module mini.platform;

import :log;

namespace mini::platform {

Interface::~Interface()
{
    g_Window.Reset();
    g_Handle.Reset();
}

bool Interface::Initialize()
{
    Module platformModule = Module(options::platformModule);
    ENSURE(platformModule.IsValid(), "failed to load platform module") return false;

    Platform* interface = platformModule.GetInterface<Platform>();
    ENSURE(interface, "platform interface not implemented") return false;

    interface->m_NativeModule = MoveArg(platformModule);
    Handle* handle = interface->CreateHandle();
    ENSURE(handle && handle->IsValid(), "failed to create platform handle") return false;

    g_Handle = UniquePtr(handle);
    Log("platform handle created");

    Window* window = interface->CreateWindow();
    ENSURE(window && window->IsValid(), "failed to create window handle") return false;

    g_Window = UniquePtr(window);
    Log("platform window created");

    return true;
}

} // namespace mini::platform