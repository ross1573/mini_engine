export module mini.graphics:render_context;

import mini.core;

namespace mini { class Graphics; }

export namespace mini::graphics
{

class GRAPHICS_API RenderContext
{
    friend class mini::Graphics;

public:
    virtual ~RenderContext() = default;

    virtual void SetViewport(Rect const&, float, float) = 0;
    virtual void SetScissorRect(RectInt const&) = 0;

    virtual void WaitForIdle() = 0;
    virtual void Execute() = 0;

protected:
    RenderContext() = default;

    virtual bool Initialize() = 0;

    virtual void BeginRender() = 0;
    virtual void EndRender() = 0;
};

} // namespace mini::grpahics