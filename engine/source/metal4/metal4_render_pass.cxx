export module mini.metal4:render_pass;

import mini.core;
import mini.graphics;
import mini.apple;

namespace mini::metal4 {

export class METAL4_API RenderPass {
private:
    SharedPtr<NS::AutoreleasePool> m_autoReleasePool;
    SharedPtr<MTL4::RenderPassDescriptor> m_renderPassDescriptor;

    MTL::Texture* m_targetTexture;
    MTL4::CommandBuffer* m_commandBuffer;
    MTL4::RenderCommandEncoder* m_renderCommandEncoder;

public:
    RenderPass(MTL4::CommandBuffer*) noexcept;
    RenderPass(RenderPass&&) noexcept = default;
    ~RenderPass() noexcept;

    bool Valid() const noexcept;
    bool Active() const noexcept;

    void Begin(MTL::Texture*, Color const&) noexcept;
    void End() noexcept;

    void SetViewport(Rect const&, float32, float32) noexcept;
    void SetScissorRect(RectInt const&) noexcept;

    MTL4::RenderCommandEncoder* GetMTL4RenderCommandEncoder() const noexcept;
    MTL4::RenderPassDescriptor* GetMTL4RenderPassDescriptor() const noexcept;

    RenderPass& operator=(RenderPass&&) noexcept = default;
};

MTL4::RenderCommandEncoder* RenderPass::GetMTL4RenderCommandEncoder() const noexcept
{
    return m_renderCommandEncoder;
}

MTL4::RenderPassDescriptor* RenderPass::GetMTL4RenderPassDescriptor() const noexcept
{
    return m_renderPassDescriptor.Get();
}

} // namespace mini::metal4