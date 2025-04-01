module;

#include <Foundation/NSAutoreleasePool.hpp>
#include <Foundation/NSNotification.hpp>

#include "assertion.h"
#include "cocoa.h"

module mini.macos;

import mini.platform;
import mini.graphics;

namespace mini::macos {

Handle::Handle()
    : m_AutoReleasePool(nullptr)
    , m_Application(nullptr)
{
    m_AutoReleasePool = NS::AutoreleasePool::alloc()->init();
}

Handle::~Handle()
{
    m_AutoReleasePool->release();
}

bool Handle::Initialize()
{
    m_Application = cocoa::CreateApplication(static_cast<cocoa::Application*>(this));
    ENSURE(m_Application, "failed to get shared NSApplication") return false;

    Run();
    return true;
}

void Handle::PollEvents()
{
}

platform::Window* Handle::CreatePlatformWindow()
{
    return new Window();
}

graphics::Device* Handle::CreateGraphicDevice(graphics::API)
{
    return nullptr;
}

void Handle::ApplicationWillFinishLaunching(NS::Notification*)
{
}

void Handle::ApplicationDidFinishLaunching(NS::Notification*)
{
    Stop();
}

} // namespace mini::macos