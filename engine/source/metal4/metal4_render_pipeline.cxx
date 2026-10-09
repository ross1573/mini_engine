module;

#include <Metal/MTL4RenderPipeline.hpp>

export module mini.metal4:render_pipeline;

import mini.core;
import mini.apple;
import mini.graphics;

export namespace MTL {

using MTL::RenderPipelineState;

} // namespace MTL

export namespace MTL4 {

using MTL4::RenderPipelineColorAttachmentDescriptor;
using MTL4::RenderPipelineDescriptor;

} // namespace MTL4

namespace mini::metal4 {

class Device;
class Compiler;

} // namespace mini::metal4

namespace mini::metal4 {

export METAL4_API SharedPtr<MTL4::RenderPipelineDescriptor> MTLRenderPipelineDescriptor(
    graphics::RenderPipelineDescriptor const& descriptor);

export class METAL4_API RenderPipelineState : public graphics::RenderPipelineState {
private:
    SharedPtr<MTL::RenderPipelineState> m_renderPipelineState;

public:
    RenderPipelineState(PtrView<Compiler> compiler, graphics::RenderPipelineDescriptor const& descriptor);

    [[nodiscard]] bool Valid() const noexcept { return m_renderPipelineState.Valid(); }
    [[nodiscard]] String Name() const { return ToString(m_renderPipelineState->label()); }

    [[nodiscard]] MTL::RenderPipelineState* MTLRenderPipelineState() const noexcept;
};

inline MTL::RenderPipelineState* RenderPipelineState::MTLRenderPipelineState() const noexcept
{
    return m_renderPipelineState.Get();
}

} // namespace mini::metal4