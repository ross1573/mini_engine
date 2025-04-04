module;

#include "cocoa_application.h"

export module mini.macos:handle;

import mini.platform;
import mini.graphics;

export namespace mini::macos {

class MACOS_API Handle final
    : public platform::Handle
    , private cocoa::Application {
private:
    NS::AutoreleasePool* m_AutoReleasePool;

public:
    Handle();
    ~Handle();

    bool Initialize() final;
    void PollEvents() final { cocoa::Application::PollEvents(); }

    platform::Window* CreatePlatformWindow() final;
    graphics::Device* CreateGraphicDevice(graphics::API) final;

    void ApplicationWillFinishLaunching(NS::Notification*) final;
    void ApplicationDidFinishLaunching(NS::Notification*) final;

    inline NSApplication* GetNSApplication() { return m_Application; }
};

} // namespace mini::macos