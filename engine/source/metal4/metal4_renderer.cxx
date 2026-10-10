export module mini.metal4:renderer;

import mini.core;
import mini.graphics;
import mini.apple;
import :shader;
import :compiler;
import :command_buffer;
import :command_queue;
import :render_pipeline;

namespace mini::metal4 {

class Device;
class CommandQueue;
class CommandBufferAllocator;
class Compiler;
class ShaderLibrary;
class ShaderFunction;
class RenderPipelineState;

} // namespace mini::metal4

namespace mini::metal4 {

export class METAL4_API Renderer final : public graphics::Renderer {
public:
    static constexpr byte maxBufferCount = 3;

private:
    Device* m_device;

    UniquePtr<CommandQueue> m_commandQueue;
    UniquePtr<CommandBufferAllocator> m_commandBufferAllocator;

    UniquePtr<Compiler> m_compiler;
    UniquePtr<ShaderLibrary> m_library;
    UniquePtr<ShaderFunction> m_vertexFunction;
    UniquePtr<ShaderFunction> m_fragmentFunction;
    UniquePtr<RenderPipelineState> m_renderPipelineState;

    FixedQueue<uint64, maxBufferCount> m_frameQueue;
    uint64 m_frameValue;

public:
    Renderer(PtrView<Device> device);

    void Prepare() final;
    void Render() final;
    void WaitForIdle() final;

    static void HandleRenderError(PtrView<NS::Error> error);
};

} // namespace mini::metal4