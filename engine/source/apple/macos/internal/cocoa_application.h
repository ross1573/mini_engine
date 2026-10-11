#ifndef COCOA_APPLICATION_H
#define COCOA_APPLICATION_H

#include "appkit.h"
#include "foundation.h"

namespace mini::cocoa {

class Application {
protected:
    NSAutoreleasePool* m_autoReleasePool;
    NSApplication* m_application;

public:
    Application();
    virtual ~Application() noexcept;

    bool valid() const noexcept { return m_application != nullptr; }

    void launch();
    void pollEvents();
    void alertError(char const*);

    virtual void applicationWillFinishLaunching(NS::Notification*) = 0;
    virtual void applicationDidFinishLaunching(NS::Notification*) = 0;

    virtual void onKeyDown(unsigned int keyCode) = 0;
};

} // namespace mini::cocoa

#endif // COCOA_APPLICATION_H