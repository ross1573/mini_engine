module;

namespace NS {

class AutoreleasePool;

} // namespace NS

class NSApplication;

export module mini.macos:handle;

import mini.platform;
import mini.graphics;

namespace mini::macos {

export class MACOS_API Handle final : public platform::Handle {
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
};

} // namespace mini::macos