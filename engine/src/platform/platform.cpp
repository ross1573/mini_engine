#include "mini_engine.h"

#ifdef PLATFORM_WINDOWS
#include "platform/windows/handle.h"
using PlatformHandleT = mini::windows::Handle;
#else
using PlatformHandleT = void;
#endif

namespace mini
{

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
        return false;;
    }

    return true;
}

void Platform::Shutdown()
{
    DELETE(m_Window);
    DELETE(m_Handle);
}

} // namespace mini