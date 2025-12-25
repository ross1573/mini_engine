module;

#include <dlfcn.h>

module mini.macos;

import mini.core;
import mini.platform;
import mini.graphics;
import mini.engine;

namespace mini::macos {

Handle::Handle()
    : cocoa::Application()
{
}

bool Handle::Initialize()
{
    ENSURE(m_Application, "failed to get shared NSApplication") {
        return false;
    }

    Launch();
    return true;
}

platform::Window* Handle::CreatePlatformWindow()
{
    return new Window(static_cast<cocoa::Application*>(this));
}

void Handle::OnKeyDown(uint32 keyCode)
{
    switch (keyCode) {
        case 0x0C: Engine::Quit(); break;
        case 0x35: Engine::Abort("escape"); break;
        default:   break;
    }
}

} // namespace mini::macos