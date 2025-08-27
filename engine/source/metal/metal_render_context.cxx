export module mini.metal:render_context;

import mini.core;
import mini.graphics;
import mini.apple;

namespace mini::metal {

export class METAL_API RenderContext final : public graphics::RenderContext {
private:
    SharedPtr<NS::AutoreleasePool> m_AutoReleasePool;
    SharedPtr<MTL::CommandQueue> m_CmdQueue;

    MTL::CommandBuffer* m_CmdBuffer;
    MTL::RenderCommandEncoder* m_CmdEncoder;
    CA::MetalDrawable* m_Drawable;

    SharedPtr<MTL::SharedEvent> m_Event;
    uint64 m_EventValue;

public:
    RenderContext(MTL::Device*);

    bool Initialize() final;

    void BeginRender() final;
    void EndRender() final;
    void WaitForIdle() final;
    void Execute() final;

    void SetViewport(Rect const&, float, float) final;
    void SetScissorRect(RectInt const&) final;

    MTL::CommandQueue* GetMTLCommandQueue() { return m_CmdQueue.Get(); }
};

} // namespace mini::metal