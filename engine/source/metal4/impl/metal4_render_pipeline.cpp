module mini.metal4;

import :render_pipeline;

namespace mini::metal4 {

SharedPtr<MTL4::RenderPipelineDescriptor> mtlRenderPipelineDescriptor(
    graphics::RenderPipelineDescriptor const& descriptor)
{
    SharedPtr<MTL4::RenderPipelineDescriptor> desc = transfer(MTL4::RenderPipelineDescriptor::alloc());
    ENSURE(desc.valid(), "failed to allocate MTL4::RenderPipelineDescriptor") {
        return nullptr;
    }

    desc->init();
    desc->setLabel(toNSString(descriptor.name).get());

    Array<graphics::RenderPipelineTargetAttachment> const& targetAttachments = descriptor.targetAttachments;
    for (size_t i = 0; i < targetAttachments.size(); ++i) {
        graphics::RenderPipelineTargetAttachment const& targetAttachment = targetAttachments[i];
        MTL4::RenderPipelineColorAttachmentDescriptor* colorDescriptor = desc->colorAttachments()->object(i);
        colorDescriptor->setPixelFormat(mtlPixelFormat(targetAttachment.pixelFormat));
    }

    ENSURE(descriptor.vertexFunction) {
    } else {
        ShaderFunction* vertexFunction = static_cast<ShaderFunction*>(descriptor.vertexFunction);
        desc->setVertexFunctionDescriptor(vertexFunction->mtlFunctionDescriptor());
    }

    ENSURE(descriptor.fragmentFunction) {
    } else {
        ShaderFunction* fragmentFunction = static_cast<ShaderFunction*>(descriptor.fragmentFunction);
        desc->setFragmentFunctionDescriptor(fragmentFunction->mtlFunctionDescriptor());
    }

    return desc;
}

RenderPipelineState::RenderPipelineState(PtrView<Compiler> compiler,
                                         graphics::RenderPipelineDescriptor const& descriptor)
{
    SharedPtr<MTL4::RenderPipelineDescriptor> desc = mtlRenderPipelineDescriptor(descriptor);
    ENSURE(desc) {
        return;
    }

    PtrView<NS::Error> error;
    MTL::RenderPipelineState* pipelineState =
        compiler->mtlCompiler()->newRenderPipelineState(desc.get(), nullptr, &error);

    m_renderPipelineState = transfer(pipelineState);
}

} // namespace mini::metal4