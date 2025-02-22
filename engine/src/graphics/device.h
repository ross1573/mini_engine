#pragma once

#include "core/utility.h"
#include "graphics/graphics.h"

namespace mini::graphics
{
class SwapChain;
class RenderContext;
}

namespace mini::graphics
{

class Device
{
    friend class Graphics;

protected:
    Device() = default;

    virtual bool Initialize() = 0;

    virtual SwapChain* CreateSwapChain() = 0;
    virtual RenderContext* CreateRenderContext() = 0;

public:
    virtual ~Device() = default;

    virtual API GetAPI() const = 0;
};

} // namespace mini::graphics