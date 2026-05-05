export module mini.metal4:render_pass;

import mini.core;
import mini.graphics;
import :buffer;

namespace mini::metal4 {

export class METAL4_API RenderPass {
private:
    SharedPtr<MTL4::RenderPassDescriptor> m_renderPassDescriptor;
    SharedPtr<MTL4::ArgumentTableDescriptor> m_argumentTableDescriptor;
    SharedPtr<MTL4::ArgumentTable> m_argumentTable;

    MTL4::CommandBuffer* m_commandBuffer;
    MTL4::RenderCommandEncoder* m_renderCommandEncoder;
    MTL::Texture* m_targetTexture;

public:
    RenderPass(MTL::Device*, MTL4::CommandBuffer*) noexcept;
    RenderPass(RenderPass&&) noexcept = default;
    ~RenderPass() noexcept;

    bool Valid() const noexcept;
    bool Active() const noexcept;

    void Begin(MTL::Texture* targetTexture, Color const& clearColor) noexcept;
    void End() noexcept;

    void DrawPrimitives(graphics::PrimitiveType primitive, uint64 vertexStart, uint64 vertexCount);

    void SetVertexBuffer(Buffer const& buffer, uint64 index);
    void SetViewport(Rect const& rect, float32 near, float32 far) noexcept;
    void SetScissorRect(RectInt const&) noexcept;

    MTL4::RenderCommandEncoder* MTL4RenderCommandEncoder() const noexcept;
    MTL4::RenderPassDescriptor* MTL4RenderPassDescriptor() const noexcept;

    RenderPass& operator=(RenderPass&&) noexcept = default;
};

MTL4::RenderCommandEncoder* RenderPass::MTL4RenderCommandEncoder() const noexcept
{
    return m_renderCommandEncoder;
}

MTL4::RenderPassDescriptor* RenderPass::MTL4RenderPassDescriptor() const noexcept
{
    return m_renderPassDescriptor.Get();
}

} // namespace mini::metal4