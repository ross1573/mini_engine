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
    Reset();
}

bool RenderCommandEncoder::Valid() const noexcept
{
    return m_renderCommandEncoder != nullptr;
}

void RenderCommandEncoder::Reset()
{
    if (m_renderCommandEncoder != nullptr) [[likely]] {
        m_renderCommandEncoder->endEncoding();
        m_renderCommandEncoder = nullptr;
    }
}

void RenderCommandEncoder::DrawPrimitives(graphics::PrimitiveType primitiveType, uint64 vertexStart, uint64 vertexCount)
{
    ASSERT(Valid());

    MTL::PrimitiveType mtlPrimitive = MTLPrimitiveType(primitiveType);

    m_renderCommandEncoder->drawPrimitives(mtlPrimitive, vertexStart, vertexCount);
}

void RenderCommandEncoder::SetPipelineState(PtrView<graphics::RenderPipelineState> state)
{
    ASSERT(Valid());
    ASSERT(state);

    PtrView<RenderPipelineState> pipelineState = StaticCast<RenderPipelineState>(state);
    m_renderCommandEncoder->setRenderPipelineState(pipelineState->MTLRenderPipelineState());
}

void RenderCommandEncoder::SetViewport(Rect const& rect, float32 near, float32 far) noexcept
{
    ASSERT(Valid());

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

void RenderCommandEncoder::SetScissorRect(RectInt const& rect) noexcept
{
    ASSERT(Valid());

    MTL::ScissorRect ScissorRect = {
        .x = static_cast<NS::UInteger>(rect.x),
        .y = static_cast<NS::UInteger>(rect.y),
        .width = static_cast<NS::UInteger>(rect.width),
        .height = static_cast<NS::UInteger>(rect.height),
    };

    m_renderCommandEncoder->setScissorRect(ScissorRect);
}

} // namespace mini::metal4