module;

#include <Metal/MTL4CommandAllocator.hpp>
#include <Metal/MTL4CommandBuffer.hpp>
#include <Metal/MTL4CommandQueue.hpp>
#include <Metal/MTLEvent.hpp>

export module mini.metal4:renderer;

import mini.core;
import mini.graphics;
import mini.apple;
import :render_pass;

export namespace MTL4 {

using MTL4::CommandAllocator;
using MTL4::CommandBuffer;
using MTL4::CommandQueue;

} // namespace MTL4

namespace mini::metal4 {

export class METAL4_API Renderer final : public graphics::Renderer {
private:
    SharedPtr<NS::AutoreleasePool> m_autoReleasePool;
    SharedPtr<MTL4::CommandQueue> m_commandQueue;
    SharedPtr<MTL4::CommandBuffer> m_commandBuffer;
    SharedPtr<MTL4::CommandAllocator> m_commandAllocator;

    Compiler m_compiler;
    ShaderLibrary m_library;

    Array<RenderPass> m_renderPasses;
    Array<RenderPipelineState> m_renderPipelineStates;

    SharedPtr<MTL::SharedEvent> m_event;
    uint64 m_eventValue;

public:
    Renderer(Device const&);

    void WaitForIdle() final;
    void Render() final;
    void Execute() final;

    [[nodiscard]] MTL4::CommandQueue* MTL4CommandQueue() const noexcept;
};

MTL4::CommandQueue* Renderer::MTL4CommandQueue() const noexcept
{
    return m_commandQueue.Get();
}

} // namespace mini::metal4