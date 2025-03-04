module;

#include "assertion.h"

module mini.platform;

import mini.core;

namespace mini
{

bool Platform::Initialize(Handle* handle)
{
    ASSERT(handle);

    g_Handle = UniquePtr(handle);
    ENSURE(g_Handle->Initialize())
    {
        log::Error("Failed to initialize platform handle");
        return false;
    }

    g_Window = UniquePtr(g_Handle->CreatePlatformWindow());
    ENSURE(g_Window->Initialize())
    {
        log::Error("Failed to initialize platform window");
        return false;
    }

    return true;
}

void Platform::Shutdown()
{
    g_Window.Reset();
    g_Handle.Reset();
}

} // namespace mini