export module mini.metal4:renderer;

import mini.core;
import mini.graphics;
import mini.apple;
import :event;
import :command_queue;
import :command_buffer;
import :render_pass;

namespace mini::metal4 {

export class METAL4_API Renderer final : public graphics::Renderer {
private:
    Device* m_device;

    UniquePtr<CommandQueue> m_commandQueue;
    UniquePtr<CommandBuffer> m_commandBuffer;
    CommandAllocatorPool m_commandAllocatorPool;

    UniquePtr<Compiler> m_compiler;
    UniquePtr<ShaderLibrary> m_library;
    UniquePtr<ShaderFunction> m_vertexFunction;
    UniquePtr<ShaderFunction> m_fragmentFunction;
    UniquePtr<RenderPipelineState> m_renderPipelineState;

    UniquePtr<SharedEvent> m_event;
    uint64 m_eventValue;

public:
    Renderer(Device* device);

    void WaitForIdle() final;
    void Render() final;
};

} // namespace mini::metal4