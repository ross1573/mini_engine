module;

#include "core/assert.h"

module mini.platform;

import mini.core;

namespace mini
{

bool Platform::Initialize(Handle* handle)
{
    ASSERT(handle);

    g_Handle = handle;
    if (!g_Handle->Initialize())
    {
        Log::Error("Failed to initialize platform handle");
        return false;
    }

    g_Window = g_Handle->CreatePlatformWindow();
    if (!g_Window->Initialize())
    {
        Log::Error("Failed to initialize platform window");
        return false;
    }

    return true;
}

void Platform::Shutdown()
{
    DELETE(g_Window);
    DELETE(g_Handle);
}

} // namespace mini