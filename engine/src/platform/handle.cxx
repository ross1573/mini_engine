module;

#include "core/define.h"

export module mini.platform:handle;

import mini.core;
import mini.graphics;
import :window;

export namespace mini::platform
{

class Handle
{
    //friend class mini::Engine;

protected:
    Handle() = default;

public:
    virtual bool Initialize() = 0;
    virtual void PollEvents() = 0;

    virtual platform::Window* CreatePlatformWindow() = 0;
    virtual graphics::Device* CreateGraphicDevice(graphics::API) = 0;

public:
    virtual ~Handle() = default;
};

} // namespace mini::platform