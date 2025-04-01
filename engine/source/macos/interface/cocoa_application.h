#pragma once

#include "foundation.hpp"

namespace mini::cocoa {

class Application {
public:
    void Run();
    void Stop();
    void PollEvents();

    virtual void ApplicationWillFinishLaunching(NS::Notification*) = 0;
    virtual void ApplicationDidFinishLaunching(NS::Notification*) = 0;
};

} // namespace mini::cocoa