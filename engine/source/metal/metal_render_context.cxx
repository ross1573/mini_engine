export module mini.metal:render_context;

import mini.core;
import mini.graphics;
import mini.apple;

namespace mini::metal {

export class METAL_API RenderContext final : public graphics::RenderContext {
private:
    SharedPtr<NS::AutoreleasePool> m_autoReleasePool;
    SharedPtr<MTL::CommandQueue> m_cmdQueue;

    MTL::CommandBuffer* m_cmdBuffer;
    MTL::RenderCommandEncoder* m_cmdEncoder;
    CA::MetalDrawable* m_drawable;

    SharedPtr<MTL::SharedEvent> m_event;
    uint64 m_eventValue;

public:
    RenderContext(MTL::Device*);

    bool Initialize() final;

    void BeginRender() final;
    void EndRender() final;
    void WaitForIdle() final;
    void Execute() final;

    void SetViewport(Rect const&, float32, float32) final;
    void SetScissorRect(RectInt const&) final;

    MTL::CommandQueue* GetMTLCommandQueue() { return m_cmdQueue.Get(); }
};

} // namespace mini::metal