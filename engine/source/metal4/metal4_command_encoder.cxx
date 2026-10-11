module;

#include <Metal/MTL4RenderCommandEncoder.hpp>

export module mini.metal4:command_encoder;

import mini.core;
import mini.graphics;

export namespace MTL {

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

using MTL4::RenderCommandEncoder;

} // namespace MTL4

namespace mini::metal4 {

export METAL4_API constexpr MTL::PrimitiveType mtlPrimitiveType(graphics::PrimitiveType primitiveType) noexcept
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

export METAL4_API constexpr MTL::ScissorRect mtlScissorRect(graphics::ScissorRect scissorRect) noexcept
{
    return MTL::ScissorRect{
        .x = static_cast<uint64>(scissorRect.x),
        .y = static_cast<uint64>(scissorRect.y),
        .width = static_cast<uint64>(scissorRect.width),
        .height = static_cast<uint64>(scissorRect.height),
    };
}

export METAL4_API constexpr MTL::Viewport mtlViewport(graphics::Viewport viewport) noexcept
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

export class METAL4_API RenderCommandEncoder {
private:
    MTL4::RenderCommandEncoder* m_renderCommandEncoder;

public:
    explicit RenderCommandEncoder(PtrView<MTL4::RenderCommandEncoder> renderCommandEncoder) noexcept;
    ~RenderCommandEncoder() noexcept;

    void reset();

    void drawPrimitives(graphics::PrimitiveType primitive, uint64 vertexStart, uint64 vertexCount);

    void setPipelineState(PtrView<graphics::RenderPipelineState> state);
    void setViewport(Rect const& rect, float32 near, float32 far) noexcept;
    void setScissorRect(RectInt const&) noexcept;

    [[nodiscard]] bool valid() const noexcept;

    [[nodiscard]] MTL4::RenderCommandEncoder* mtlRenderCommandEncoder() const noexcept;
};

MTL4::RenderCommandEncoder* RenderCommandEncoder::mtlRenderCommandEncoder() const noexcept
{
    return m_renderCommandEncoder;
}

} // namespace mini::metal4