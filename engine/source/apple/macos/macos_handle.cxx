module;

#include "cocoa_application.h"

export module mini.macos:handle;

import mini.platform;
import mini.graphics;

namespace mini::macos {

export class MACOS_API Handle final
    : public platform::Handle
    , private cocoa::Application {
public:
    Handle();

    bool Initialize() final;
    void PollEvents() final { cocoa::Application::PollEvents(); }

    platform::Window* CreatePlatformWindow() final;
    graphics::Device* CreateGraphicDevice(graphics::API) final;

    inline NSApplication* GetNSApplication() { return m_Application; }

private:
    void ApplicationWillFinishLaunching(NS::Notification*) final {}
    void ApplicationDidFinishLaunching(NS::Notification*) final {}

    void Quit() final;
    void Abort() final;
};

} // namespace mini::macos