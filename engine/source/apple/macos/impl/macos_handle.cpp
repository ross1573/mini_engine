module;

#include <dlfcn.h>

module mini.macos;

import mini.core;
import mini.platform;
import mini.graphics;
import mini.engine;
import :modules;

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

platform::Module* Handle::LoadModule(StringView name)
{
    return new Module(name);
}

platform::Window* Handle::CreatePlatformWindow()
{
    return new Window(static_cast<cocoa::Application*>(this));
}

graphics::Device* Handle::CreateGraphicDevice(graphics::API api)
{
    platform::Module* moduleHandle = nullptr;
    graphics::Device* (*funcHandle)() = nullptr;

    switch (api) {
        case graphics::API::Metal: moduleHandle = LoadModule("metal"); break;

        default: break;
    }

    ENSURE(moduleHandle, "failed to load graphics module") {
        return nullptr;
    }
    m_GraphicsModule = UniquePtr(moduleHandle);

    funcHandle = m_GraphicsModule->GetFunction<graphics::Device*>("CreateGraphicDevice");
    if (funcHandle == nullptr) {
        return nullptr;
    }

    return funcHandle();
}

void Handle::OnKeyDown(uint keyCode)
{
    switch (keyCode) {
        case 0x0C: Engine::Quit(); break;
        case 0x35: Engine::Abort("escape"); break;
        default:   break;
    }
}

} // namespace mini::macos