module;

#include "core/define.h"
#include "core/assert.h"
#include "core/macro.h"

module mini.platform;

import mini.core;
import mini.log;

#ifdef PLATFORM_WINDOWS
import mini.windows;
using PlatformHandleT = mini::windows::Handle;
#else
using PlatformHandleT = void;
#endif

namespace mini
{

Platform::Handle* Platform::m_Handle = nullptr;
Platform::Window* Platform::m_Window = nullptr;

bool Platform::Initialize()
{
    m_Handle = new PlatformHandleT();
    if (!m_Handle->Initialize())
    {
        Log::Error("Failed to initialize platform handle");
        return false;
    }

    m_Window = m_Handle->CreatePlatformWindow();
    if (!m_Window->Initialize())
    {
        Log::Error("Failed to initialize platform window");
        return false;
    }

    return true;
}

void Platform::Shutdown()
{
    DELETE(m_Window);
    DELETE(m_Handle);
}

} // namespace mini