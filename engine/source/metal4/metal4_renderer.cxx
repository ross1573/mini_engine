export module mini.metal4:renderer;

import mini.core;
import mini.graphics;
import mini.apple;

namespace mini::metal4 {

export class METAL4_API Renderer final : public graphics::Renderer {
private:
    SharedPtr<NS::AutoreleasePool> m_autoReleasePool;
    SharedPtr<MTL4::CommandQueue> m_cmdQueue;
    SharedPtr<MTL4::CommandBuffer> m_cmdBuffer;
    SharedPtr<MTL4::CommandAllocator> m_cmdAllocator;

    MTL4::RenderCommandEncoder* m_cmdEncoder;
    CA::MetalDrawable* m_drawable;

    SharedPtr<MTL::SharedEvent> m_event;
    uint64 m_eventValue;

public:
    Renderer(MTL::Device*);

    bool Initialize() final;

    void BeginRender() final;
    void EndRender() final;
    void WaitForIdle() final;
    void Execute() final;

    void SetViewport(Rect const&, float32, float32) final;
    void SetScissorRect(RectInt const&) final;

    MTL4::CommandQueue* GetMTLCommandQueue() { return m_cmdQueue.Get(); }
};

} // namespace mini::metal4