export module mini.platform:handle;

import mini.core;
import mini.graphics;
import :window;

namespace mini
{

class Engine;
class Platform;

} // namespace mini

export namespace mini::platform
{

class PLATFORM_API Handle
{
    friend class mini::Engine;
    friend class mini::Platform;

public:
    virtual ~Handle() = default;

protected:
    Handle() = default;

    virtual bool Initialize() = 0;
    virtual void PollEvents() = 0;

    virtual platform::Window* CreatePlatformWindow() = 0;
    virtual graphics::Device* CreateGraphicDevice(graphics::API) = 0;
};

} // namespace mini::platform