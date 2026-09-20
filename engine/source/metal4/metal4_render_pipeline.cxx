module;

#include <Metal/MTL4FunctionDescriptor.hpp>
#include <Metal/MTL4RenderPipeline.hpp>

export module mini.metal4:render_pipeline;

import mini.core;
import mini.apple;
import :device;
import :shader;
import :compiler;

export namespace MTL4 {

using MTL4::RenderPipelineDescriptor;

} // namespace MTL4

namespace mini::metal4 {

export class METAL4_API RenderPipelineDescriptor {
private:
    SharedPtr<MTL4::RenderPipelineDescriptor> m_renderPipelineDescriptor;
    ShaderFunction m_vertex;
    ShaderFunction m_fragment;

public:
    RenderPipelineDescriptor();
    RenderPipelineDescriptor(ShaderFunction const& vertex, ShaderFunction const& fragment);
    RenderPipelineDescriptor(ShaderFunction&& vertex, ShaderFunction&& fragment);

    void SetName(StringView name);
    void SetVertexFunction(ShaderFunction const& vertex);
    void SetVertexFunction(ShaderFunction&& vertex);
    void SetFragmentFunction(ShaderFunction const& fragment);
    void SetFragmentFunction(ShaderFunction&& fragment);

    [[nodiscard]] String Name() const { return ToString(m_renderPipelineDescriptor->label()); }
    [[nodiscard]] ShaderFunction VertexFunction() const { return m_vertex; }
    [[nodiscard]] ShaderFunction FragmentFunction() const { return m_fragment; }

    [[nodiscard]] MTL4::RenderPipelineDescriptor* MTL4RenderPipelineDescriptor() const noexcept;
    [[nodiscard]] MTL4::FunctionDescriptor* MTL4VertexFunction() const noexcept;
    [[nodiscard]] MTL4::FunctionDescriptor* MTL4FragmentFunction() const noexcept;
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

public:
    RenderPipelineState(Compiler const& compiler, RenderPipelineDescriptor const& descriptor);

    [[nodiscard]] bool Valid() const noexcept { return m_renderPipelineState.Valid(); }
    [[nodiscard]] String Name() const { return ToString(m_renderPipelineState->label()); }

    [[nodiscard]] MTL::RenderPipelineState* MTLRenderPipelineState() const noexcept;
    [[nodiscard]] MTL::RenderPipelineState* operator->() const noexcept;
};

inline MTL::RenderPipelineState* RenderPipelineState::MTLRenderPipelineState() const noexcept
{
    return m_renderPipelineState.Get();
}

inline MTL::RenderPipelineState* RenderPipelineState::operator->() const noexcept
{
    return m_renderPipelineState.operator->();
}

} // namespace mini::metal4