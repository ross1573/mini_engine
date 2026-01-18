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
    virtual ~Application() noexcept;

    bool Valid() const noexcept;

    void Launch();
    void PollEvents();
    void AlertError(char const*);

    virtual void ApplicationWillFinishLaunching(NS::Notification*) = 0;
    virtual void ApplicationDidFinishLaunching(NS::Notification*) = 0;

    virtual void OnKeyDown(unsigned int keyCode) = 0;
};

inline bool Application::Valid() const noexcept
{
    return m_Application != nullptr;
}

} // namespace mini::cocoa

#endif // COCOA_APPLICATION_H