module;

#include <Foundation/Foundation.hpp>
#include <dlfcn.h>

#include "assertion.h"
#include "cocoa_application.h"

module mini.macos;

import mini.core;
import mini.platform;
import mini.graphics;

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

    Run();
    return true;
}

platform::Window* Handle::CreatePlatformWindow()
{
    return new Window();
}

graphics::Device* Handle::CreateGraphicDevice(graphics::API api)
{
    char const* graphicModulePath = "lib" LIB_PREFIX ".metal.dylib";
    void* graphicModule = nullptr;
    void* createDeviceAddr = nullptr;

    switch (api) {
        case graphics::API::Metal:
            graphicModule = dlopen(graphicModulePath, RTLD_NOW | RTLD_LOCAL);
            break;

        default: break;
    }

    ENSURE(graphicModule) {
        log::Error("failed to load module {}", graphicModulePath);
        return nullptr;
    }

    createDeviceAddr = dlsym(graphicModule, "CreateGraphicDevice");
    ENSURE(createDeviceAddr, "unable to find graphics module init function") {
        return nullptr;
    }

    typedef graphics::Device* (*CreateDeviceFuncT)();
    CreateDeviceFuncT createDeviceFunc = reinterpret_cast<CreateDeviceFuncT>(createDeviceAddr);
    return createDeviceFunc();
}

void Handle::ApplicationWillFinishLaunching(NS::Notification*)
{
}

void Handle::ApplicationDidFinishLaunching(NS::Notification*)
{
    Stop();
}

} // namespace mini::macos