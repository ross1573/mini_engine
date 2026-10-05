module mini.metal4;

import mini.core;
import mini.apple;
import :render_pipeline;

namespace mini::metal4 {

SharedPtr<MTL4::RenderPipelineDescriptor> MTLRenderPipelineDescriptor(
    graphics::RenderPipelineDescriptor const& descriptor)
{
    SharedPtr<MTL4::RenderPipelineDescriptor> desc = TransferShared(MTL4::RenderPipelineDescriptor::alloc());
    ENSURE(desc.Valid(), "failed to allocate MTL4::RenderPipelineDescriptor") {
        return nullptr;
    }

    desc->init();
    desc->setLabel(ToNSString(descriptor.name).Get());

    Array<graphics::RenderPipelineTargetAttachment> const& targetAttachments = descriptor.targetAttachments;
    for (size_t i = 0; i < targetAttachments.Size(); ++i) {
        graphics::RenderPipelineTargetAttachment const& targetAttachment = targetAttachments[i];
        MTL4::RenderPipelineColorAttachmentDescriptor* colorDescriptor = desc->colorAttachments()->object(i);
        colorDescriptor->setPixelFormat(MTLPixelFormat(targetAttachment.pixelFormat));
    }

    ENSURE(descriptor.vertexFunction) {
    } else {
        ShaderFunction* vertexFunction = static_cast<ShaderFunction*>(descriptor.vertexFunction);
        desc->setVertexFunctionDescriptor(vertexFunction->MTLFunctionDescriptor());
    }

    ENSURE(descriptor.fragmentFunction) {
    } else {
        ShaderFunction* fragmentFunction = static_cast<ShaderFunction*>(descriptor.fragmentFunction);
        desc->setFragmentFunctionDescriptor(fragmentFunction->MTLFunctionDescriptor());
    }

    return desc;
}

RenderPipelineState::RenderPipelineState(PtrView<Compiler> compiler,
                                         graphics::RenderPipelineDescriptor const& descriptor)
{
    SharedPtr<MTL4::RenderPipelineDescriptor> desc = MTLRenderPipelineDescriptor(descriptor);
    ENSURE(desc) {
        return;
    }

    PtrView<NS::Error> error;
    MTL::RenderPipelineState* pipelineState =
        compiler->MTLCompiler()->newRenderPipelineState(desc.Get(), nullptr, &error);

    m_renderPipelineState = TransferShared(pipelineState);
}

} // namespace mini::metal4