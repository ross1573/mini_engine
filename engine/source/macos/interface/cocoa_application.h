#pragma once

#include "appkit.h"
#include "foundation.h"

namespace mini::cocoa {

class Application {
protected:
    NSApplication* m_Application;

public:
    Application();

    void Run();
    void Stop();
    void PollEvents();

    virtual void ApplicationWillFinishLaunching(NS::Notification*) = 0;
    virtual void ApplicationDidFinishLaunching(NS::Notification*) = 0;
};

} // namespace mini::cocoa