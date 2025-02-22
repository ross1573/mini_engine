#pragma once

#include "math/rect.h"
#include "math/rect_int.h"
#include "graphics/graphics.h"

namespace mini::graphics
{

class RenderContext
{
    friend class mini::Graphics;

protected:
    RenderContext() = default;

    virtual bool Initialize() = 0;

    virtual void BeginRender() = 0;
    virtual void EndRender() = 0;

public:
    virtual ~RenderContext() = default;

    virtual void SetViewport(Rect const&, float, float) = 0;
    virtual void SetScissorRect(RectInt const&) = 0;

    virtual void WaitForIdle() = 0;
    virtual void Execute() = 0;
};

}