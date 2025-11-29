#ifndef COCOA_APPLICATION_H
#define COCOA_APPLICATION_H

#include "appkit.h"
#include "foundation.h"

namespace mini::cocoa {

class Application {
protected:
    NSAutoreleasePool* m_AutoReleasePool;
    NSApplication* m_Application;

public:
    Application();
    ~Application();

    void Launch();
    void PollEvents();

    virtual void ApplicationWillFinishLaunching(NS::Notification*) = 0;
    virtual void ApplicationDidFinishLaunching(NS::Notification*) = 0;

    virtual void OnKeyDown(unsigned int keyCode) = 0;
};

} // namespace mini::cocoa

#endif // COCOA_APPLICATION_H