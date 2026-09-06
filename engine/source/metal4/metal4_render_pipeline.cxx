export module mini.metal4:render_pipeline;

import mini.core;
import mini.apple;
import :device;
import :shader;
import :compiler;

namespace mini::metal4 {

export class METAL4_API RenderPipelineDescriptor {
private:
    SharedPtr<MTL4::RenderPipelineDescriptor> m_renderPipelineDescriptor;
    ShaderFunction m_vertex;
    ShaderFunction m_fragment;
    String m_name;

public:
    RenderPipelineDescriptor();
    RenderPipelineDescriptor(ShaderFunction const& vertex, ShaderFunction const& fragment);
    RenderPipelineDescriptor(ShaderFunction&& vertex, ShaderFunction&& fragment);

    void SetName(StringView name);
    void SetVertexFunction(ShaderFunction const& vertex);
    void SetVertexFunction(ShaderFunction&& vertex);
    void SetFragmentFunction(ShaderFunction const& fragment);
    void SetFragmentFunction(ShaderFunction&& fragment);

    String Name() const { return m_name; }
    ShaderFunction VertexFunction() const { return m_vertex; }
    ShaderFunction FragmentFunction() const { return m_fragment; }

    MTL4::RenderPipelineDescriptor* MTL4RenderPipelineDescriptor() const noexcept;
    MTL4::FunctionDescriptor* MTL4VertexFunction() const noexcept;
    MTL4::FunctionDescriptor* MTL4FragmentFunction() const noexcept;
};

inline MTL4::RenderPipelineDescriptor* RenderPipelineDescriptor::MTL4RenderPipelineDescriptor() const noexcept
{
    return m_renderPipelineDescriptor.Get();
}

inline MTL4::FunctionDescriptor* RenderPipelineDescriptor::MTL4VertexFunction() const noexcept
{
    return m_vertex.MTL4FunctionDescriptor();
}

inline MTL4::FunctionDescriptor* RenderPipelineDescriptor::MTL4FragmentFunction() const noexcept
{
    return m_fragment.MTL4FunctionDescriptor();
}

export class METAL4_API RenderPipelineState {
private:
    SharedPtr<MTL::RenderPipelineState> m_renderPipelineState;
    String m_name;

public:
    RenderPipelineState(Compiler const& compiler, RenderPipelineDescriptor const& descriptor);

    bool Valid() const noexcept { return m_renderPipelineState.Valid(); }
    String Name() const { return m_name; }

    MTL::RenderPipelineState* MTLRenderPipelineState() const noexcept { return m_renderPipelineState.Get(); }
    MTL::RenderPipelineState* operator->() const noexcept { return m_renderPipelineState.operator->(); }
};

} // namespace mini::metal4