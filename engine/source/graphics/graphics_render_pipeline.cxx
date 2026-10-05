export module mini.graphics:render_pipeline;

import mini.core;
import :shader;
import :common;

namespace mini::graphics {

export struct GRAPHICS_API RenderPipelineTargetAttachment {
public:
    PixelFormat pixelFormat = PixelFormat::RGBA8unorm;
};

export struct GRAPHICS_API RenderPipelineDescriptor {
public:
    Array<RenderPipelineTargetAttachment> targetAttachments;
    ShaderFunction* vertexFunction = nullptr;
    ShaderFunction* fragmentFunction = nullptr;
    String name;

public:
    constexpr RenderPipelineDescriptor() noexcept = default;

    constexpr RenderPipelineDescriptor(ArrayView<RenderPipelineTargetAttachment> attachments,
                                       PtrView<ShaderFunction> vertex,
                                       PtrView<ShaderFunction> fragment)
        : targetAttachments(attachments)
        , vertexFunction(vertex)
        , fragmentFunction(fragment)
    {
    }

    constexpr RenderPipelineDescriptor(ArrayView<RenderPipelineTargetAttachment> attachments,
                                       PtrView<ShaderFunction> vertex,
                                       PtrView<ShaderFunction> fragment,
                                       StringView name)
        : targetAttachments(attachments)
        , vertexFunction(vertex)
        , fragmentFunction(fragment)
        , name(name)
    {
    }
};

export class GRAPHICS_API RenderPipelineState {
public:
    virtual ~RenderPipelineState() noexcept = 0;
};

} // namespace mini::graphics