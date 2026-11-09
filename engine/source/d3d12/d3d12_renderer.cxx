export module mini.d3d12:renderer;

import mini.core;
import mini.graphics;
import mini.windows;
import :event;
import :command_buffer;
import :command_queue;

namespace mini::d3d12 {

class SwapChainBuffer;

} // namespace mini::d3d12

export namespace mini::d3d12 {

class Renderer final : public graphics::Renderer {
private:
    Device* m_device;

    UniquePtr<CommandQueue> m_commandQueue;
    UniquePtr<CommandBufferPool> m_commandBufferPool;

    uint64 m_frameValue;

public:
    Renderer(PtrView<Device> device);
    ~Renderer();

    void prepare() final;
    void render() final;
    void waitIdle() final;

    [[nodiscard]] CommandQueue* commandQueue() const { return m_commandQueue.get(); }
};

} // namespace mini::d3d12