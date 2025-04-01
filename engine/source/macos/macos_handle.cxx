module;

#include "appkit.h"
#include "cocoa_application.h"
#include "foundation.hpp"

export module mini.macos:handle;

import mini.platform;
import mini.graphics;

export namespace mini::macos {

class MACOS_API Handle final
    : public platform::Handle
    , public cocoa::Application {
private:
    NS::AutoreleasePool* m_AutoReleasePool;
    NSApplication* m_Application;

public:
    Handle();
    ~Handle();

    bool Initialize() final;
    void PollEvents() final;

    platform::Window* CreatePlatformWindow() final;
    graphics::Device* CreateGraphicDevice(graphics::API) final;

    void ApplicationWillFinishLaunching(NS::Notification*) final;
    void ApplicationDidFinishLaunching(NS::Notification*) final;
};

} // namespace mini::macos