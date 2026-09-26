module mini.metal4;

import mini.core;
import mini.apple;
import :render_pipeline;

namespace mini::metal4 {

RenderPipelineDescriptor::RenderPipelineDescriptor()
{
    constexpr MTL::PixelFormat pixelFormat = MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB;

    m_renderPipelineDescriptor = TransferShared(MTL4::RenderPipelineDescriptor::alloc());
    m_renderPipelineDescriptor->init();
    m_renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(pixelFormat);
}

RenderPipelineDescriptor::RenderPipelineDescriptor(ShaderFunction* vertex, ShaderFunction* fragment)
    : RenderPipelineDescriptor()
{
    SetVertexFunction(vertex);
    SetFragmentFunction(fragment);
}

void RenderPipelineDescriptor::SetName(StringView name)
{
    SharedPtr<NS::String> label = ToNSString(name);
    m_renderPipelineDescriptor->setLabel(label.Get());
}

void RenderPipelineDescriptor::SetVertexFunction(ShaderFunction* vertex)
{
    ENSURE(vertex, "invalid vertex function") {
        return;
    }

    m_vertex = vertex;
    m_renderPipelineDescriptor->setVertexFunctionDescriptor(m_vertex->MTLFunctionDescriptor());
}

void RenderPipelineDescriptor::SetFragmentFunction(ShaderFunction* fragment)
{
    ENSURE(fragment, "invalid fragment function") {
        return;
    }

    m_fragment = fragment;
    m_renderPipelineDescriptor->setFragmentFunctionDescriptor(m_fragment->MTLFunctionDescriptor());
}

RenderPipelineState::RenderPipelineState(Compiler* compiler, RenderPipelineDescriptor* descriptor)
{
    NS::Error* error;
    MTL4::RenderPipelineDescriptor* desc = descriptor->MTLRenderPipelineDescriptor();
    m_renderPipelineState = TransferShared(compiler->MTLCompiler()->newRenderPipelineState(desc, nullptr, &error));
    ENSURE(m_renderPipelineState, error, "failed to create render pipeline state") {
        return;
    }
}

} // namespace mini::metal4