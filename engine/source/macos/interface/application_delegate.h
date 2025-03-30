#pragma once

#include "foundation.hpp"

class ApplicationDelegate {
public:
    void Run();
    void Stop();

    virtual void ApplicationWillFinishLaunching(NS::Notification*) = 0;
    virtual void ApplicationDidFinishLaunching(NS::Notification*) = 0;
};