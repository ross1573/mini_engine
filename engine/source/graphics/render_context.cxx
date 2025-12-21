export module mini.graphics:render_context;

import mini.math;

namespace mini::graphics {

export class GRAPHICS_API RenderContext {
public:
    virtual ~RenderContext() = default;

    virtual bool Initialize() = 0;

    virtual void BeginRender() = 0;
    virtual void EndRender() = 0;
    virtual void WaitForIdle() = 0;
    virtual void Execute() = 0;

    virtual void SetViewport(Rect const&, float32, float32) = 0;
    virtual void SetScissorRect(RectInt const&) = 0;

protected:
    RenderContext() = default;
};

} // namespace mini::graphics