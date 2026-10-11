module mini.metal4;

import :command_encoder;

namespace mini::metal4 {

RenderCommandEncoder::RenderCommandEncoder(PtrView<MTL4::RenderCommandEncoder> renderCommandEncoder) noexcept
    : m_renderCommandEncoder(renderCommandEncoder)
{
    ASSERT(renderCommandEncoder);
}

RenderCommandEncoder::~RenderCommandEncoder() noexcept
{
    reset();
}

bool RenderCommandEncoder::valid() const noexcept
{
    return m_renderCommandEncoder != nullptr;
}

void RenderCommandEncoder::reset()
{
    if (m_renderCommandEncoder != nullptr) [[likely]] {
        m_renderCommandEncoder->endEncoding();
        m_renderCommandEncoder = nullptr;
    }
}

void RenderCommandEncoder::drawPrimitives(graphics::PrimitiveType primitiveType, uint64 vertexStart, uint64 vertexCount)
{
    ASSERT(valid());

    MTL::PrimitiveType mtlPrimitive = mtlPrimitiveType(primitiveType);

    m_renderCommandEncoder->drawPrimitives(mtlPrimitive, vertexStart, vertexCount);
}

void RenderCommandEncoder::setPipelineState(PtrView<graphics::RenderPipelineState> state)
{
    ASSERT(valid());
    ASSERT(state);

    PtrView<RenderPipelineState> pipelineState = staticCast<RenderPipelineState>(state);
    m_renderCommandEncoder->setRenderPipelineState(pipelineState->mtlRenderPipelineState());
}

void RenderCommandEncoder::setViewport(Rect const& rect, float32 near, float32 far) noexcept
{
    ASSERT(valid());

    MTL::Viewport viewport = {
        .originX = static_cast<double>(rect.x),
        .originY = static_cast<double>(rect.y),
        .width = static_cast<double>(rect.width),
        .height = static_cast<double>(rect.height),
        .znear = static_cast<double>(near),
        .zfar = static_cast<double>(far),
    };

    m_renderCommandEncoder->setViewport(viewport);
}

void RenderCommandEncoder::setScissorRect(RectInt const& rect) noexcept
{
    ASSERT(valid());

    MTL::ScissorRect scissorRect = {
        .x = static_cast<NS::UInteger>(rect.x),
        .y = static_cast<NS::UInteger>(rect.y),
        .width = static_cast<NS::UInteger>(rect.width),
        .height = static_cast<NS::UInteger>(rect.height),
    };

    m_renderCommandEncoder->setScissorRect(scissorRect);
}

} // namespace mini::metal4