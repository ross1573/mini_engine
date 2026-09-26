module;

#include <Metal/MTL4ArgumentTable.hpp>
#include <Metal/MTL4RenderCommandEncoder.hpp>
#include <Metal/MTL4RenderPass.hpp>

export module mini.metal4:render_pass;

import mini.core;
import mini.graphics;
import :buffer;
import :texture;
import :command_buffer;
import :render_pipeline;

export namespace MTL {

using MTL::ClearColor;
using MTL::LoadAction;
using MTL::StoreAction;

using MTL::RenderPassColorAttachmentDescriptor;
using MTL::RenderPassColorAttachmentDescriptorArray;
using MTL::RenderPassDepthAttachmentDescriptor;
using MTL::RenderPassDescriptor;
using MTL::RenderPassStencilAttachmentDescriptor;

using MTL::RenderStageFragment;
using MTL::RenderStageMesh;
using MTL::RenderStageObject;
using MTL::RenderStages;
using MTL::RenderStageTile;
using MTL::RenderStageVertex;

using MTL::PrimitiveType;
using MTL::ScissorRect;
using MTL::Viewport;

} // namespace MTL

export namespace MTL4 {

using MTL4::ArgumentTable;
using MTL4::ArgumentTableDescriptor;
using MTL4::RenderCommandEncoder;
using MTL4::RenderPassDescriptor;

} // namespace MTL4

namespace mini::metal4 {

export METAL4_API constexpr MTL::PrimitiveType MTLPrimitiveType(graphics::PrimitiveType primitiveType) noexcept
{
    switch (primitiveType) {
        case graphics::PrimitiveType::Point:    return MTL::PrimitiveType::PrimitiveTypePoint;
        case graphics::PrimitiveType::Line:     return MTL::PrimitiveType::PrimitiveTypeLine;
        case graphics::PrimitiveType::Triangle: return MTL::PrimitiveType::PrimitiveTypeTriangle;
    }

    ASSERT(primitiveType == graphics::PrimitiveType::Triangle,
           "invalid primitive type {}.",
           static_cast<byte>(primitiveType));

    return MTL::PrimitiveType::PrimitiveTypeTriangle;
}

export METAL4_API constexpr MTL::ScissorRect MTLScissorRect(graphics::ScissorRect scissorRect) noexcept
{
    return MTL::ScissorRect{
        .x = static_cast<uint64>(scissorRect.x),
        .y = static_cast<uint64>(scissorRect.y),
        .width = static_cast<uint64>(scissorRect.width),
        .height = static_cast<uint64>(scissorRect.height),
    };
}

export METAL4_API constexpr MTL::Viewport MTLViewport(graphics::Viewport viewport) noexcept
{
    return MTL::Viewport{
        .originX = static_cast<float64>(viewport.x),
        .originY = static_cast<float64>(viewport.y),
        .width = static_cast<float64>(viewport.width),
        .height = static_cast<float64>(viewport.height),
        .znear = static_cast<float64>(viewport.near),
        .zfar = static_cast<float64>(viewport.far),
    };
}

export METAL4_API constexpr MTL::ClearColor MTLClearColor(Color color) noexcept
{
    return MTL::ClearColor{
        static_cast<double>(color.r),
        static_cast<double>(color.g),
        static_cast<double>(color.b),
        static_cast<double>(color.a),
    };
}

export METAL4_API constexpr MTL::LoadAction MTLLoadAction(graphics::LoadAction loadAction) noexcept
{
    switch (loadAction) {
        case graphics::LoadAction::Undefined: return MTL::LoadActionDontCare;
        case graphics::LoadAction::Load:      return MTL::LoadActionLoad;
        case graphics::LoadAction::Clear:     return MTL::LoadActionClear;
    }

    ASSERT(loadAction == graphics::LoadAction::Undefined, "invalid load action {}.", static_cast<byte>(loadAction));
    return MTL::LoadActionDontCare;
}

export METAL4_API constexpr MTL::StoreAction MTLStoreAction(graphics::StoreAction storeAction) noexcept
{
    switch (storeAction) {
        case graphics::StoreAction::Undefined: return MTL::StoreActionDontCare;
        case graphics::StoreAction::Store:     return MTL::StoreActionStore;
    }

    ASSERT(storeAction == graphics::StoreAction::Undefined, "invalid store action {}.", static_cast<byte>(storeAction));
    return MTL::StoreActionDontCare;
}

export METAL4_API SharedPtr<MTL4::RenderPassDescriptor> MTLRenderPassDescriptor(
    graphics::RenderPassDescriptor const& descriptor);

export class METAL4_API RenderPass {
private:
    CommandBuffer* m_commandBuffer;
    MTL4::RenderCommandEncoder* m_renderCommandEncoder;

public:
    RenderPass(CommandBuffer* commandBuffer, graphics::RenderPassDescriptor const& descriptor) noexcept;
    RenderPass(RenderPass&& other) noexcept = default;
    ~RenderPass() noexcept;

    [[nodiscard]] bool Valid() const noexcept;

    void DrawPrimitives(graphics::PrimitiveType primitive, uint64 vertexStart, uint64 vertexCount);

    void SetPipelineState(RenderPipelineState* state);
    void SetViewport(Rect const& rect, float32 near, float32 far) noexcept;
    void SetScissorRect(RectInt const&) noexcept;

    [[nodiscard]] MTL4::RenderCommandEncoder* MTLRenderCommandEncoder() const noexcept;

    RenderPass& operator=(RenderPass&& other) noexcept = default;

public:
    RenderPass(RenderPass const& other) = deleted_function("render pass should be unique");
    RenderPass& operator=(RenderPass const& other) = deleted_function("render pass should be unique");
};

MTL4::RenderCommandEncoder* RenderPass::MTLRenderCommandEncoder() const noexcept
{
    return m_renderCommandEncoder;
}

} // namespace mini::metal4