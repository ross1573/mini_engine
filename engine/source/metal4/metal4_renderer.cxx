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
public:
    static constexpr byte maxBufferCount = 3;

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
    FixedQueue<uint64, maxBufferCount> m_eventQueue;
    uint64 m_eventValue;
    uint64 m_frameValue;

public:
    Renderer(PtrView<Device> device);

    void Prepare() final;
    void Render() final;
    void WaitForIdle() final;

    static void HandleRenderError(PtrView<NS::Error> error);
};

} // namespace mini::metal4