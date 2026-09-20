module;

#include <Metal/MTL4ArgumentTable.hpp>
#include <Metal/MTL4CommandBuffer.hpp>
#include <Metal/MTL4RenderPass.hpp>

export module mini.metal4:render_pass;

import mini.core;
import mini.graphics;
import :buffer;
import :texture;
import :render_pipeline;

export namespace MTL {

using MTL::ClearColor;
using MTL::LoadAction;
using MTL::RenderPassColorAttachmentDescriptor;
using MTL::RenderStageFragment;
using MTL::RenderStageMesh;
using MTL::RenderStageObject;
using MTL::RenderStages;
using MTL::RenderStageTile;
using MTL::RenderStageVertex;
using MTL::ScissorRect;
using MTL::StoreAction;
using MTL::Viewport;

} // namespace MTL

export namespace MTL4 {

using MTL4::ArgumentTable;
using MTL4::ArgumentTableDescriptor;
using MTL4::RenderCommandEncoder;
using MTL4::RenderPassDescriptor;

} // namespace MTL4

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
    RenderPass(Device const&, MTL4::CommandBuffer*) noexcept;
    RenderPass(RenderPass&&) noexcept = default;
    ~RenderPass() noexcept;

    [[nodiscard]] bool Valid() const noexcept;
    [[nodiscard]] bool Active() const noexcept;

    void Begin(MTL::Texture* targetTexture, Color const& clearColor) noexcept;
    void End() noexcept;

    void DrawPrimitives(graphics::PrimitiveType primitive, uint64 vertexStart, uint64 vertexCount);

    void SetVertexBuffer(Buffer const& buffer, uint64 index);
    void SetPipelineState(RenderPipelineState const& state);
    void SetViewport(Rect const& rect, float32 near, float32 far) noexcept;
    void SetScissorRect(RectInt const&) noexcept;

    [[nodiscard]] MTL4::RenderCommandEncoder* MTL4RenderCommandEncoder() const noexcept;
    [[nodiscard]] MTL4::RenderPassDescriptor* MTL4RenderPassDescriptor() const noexcept;

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