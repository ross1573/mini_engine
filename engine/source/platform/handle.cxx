export module mini.platform:handle;

import mini.core;
import mini.graphics;
import :window;

namespace mini::platform {

export class PLATFORM_API Handle {
public:
    virtual ~Handle() = default;

    virtual bool Initialize() = 0;
    virtual void PollEvents() = 0;

    virtual platform::Window* CreatePlatformWindow() = 0;
    virtual graphics::Device* CreateGraphicDevice(graphics::API api) = 0;

protected:
    Handle() = default;
};

} // namespace mini::platform