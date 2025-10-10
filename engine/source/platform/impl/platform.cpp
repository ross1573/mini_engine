module mini.platform;

import mini.core;
import :log;

namespace mini {

bool Platform::Initialize(Handle* handle)
{
    ASSERT(handle);

    g_Handle = UniquePtr(handle);
    ENSURE(g_Handle->Initialize(), "Failed to initialize platform handle") {
        return false;
    }
    platform::Log("platform handle initialized");

    g_Window = UniquePtr(g_Handle->CreatePlatformWindow());
    platform::Log("platform window initialized");

    return true;
}

void Platform::Shutdown()
{
    g_Window.Reset();
    g_Handle.Reset();
}

} // namespace mini