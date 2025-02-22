#pragma once

#include "graphics/graphics.h"

namespace mini { class Engine; }
namespace mini::platform { class Window; }
namespace mini::graphics { class Device; }

namespace mini::platform
{

class Handle
{
    friend class mini::Platform;
    friend class mini::Engine;

protected:
    Handle() = default;

    virtual bool Initialize() = 0;
    virtual void PollEvents() = 0;

    virtual platform::Window* CreatePlatformWindow() = 0;
    virtual graphics::Device* CreateGraphicDevice(graphics::API) = 0;

public:
    virtual ~Handle() = default;
};

} // namespace mini::platform