export module mini.d3d12:command_queue;

import mini.core;
import mini.graphics;
import mini.windows;
import :event;
import :common;

D3D12_EXPORT(ID3D12CommandQueue);
D3D12_EXPORT(ID3D12CommandList);

namespace mini::d3d12 {

class CommandBuffer;

} // namespace mini::d3d12

namespace mini::d3d12 {

export class CommandQueue {
private:
    SharedPtr<ID3D12CommandQueue> m_commandQueue;

    Event m_event;
    uint64 m_eventValue;

    Array<ID3D12CommandList*> m_commitBuffer;

public:
    CommandQueue(PtrView<Device> device, CommandType commandType);

    void commit(PtrView<CommandBuffer const> commandBuffer);
    void commit(ArrayView<CommandBuffer const*> commandBuffers);

    void wait(PtrView<CommandQueue const> commandQueue);
    void signal(PtrView<Event const> event, uint64 value);

    [[nodiscard]] bool valid() const noexcept { return m_commandQueue.valid(); }
    [[nodiscard]] uint64 eventValue() const noexcept { return m_eventValue; }

    [[nodiscard]] ID3D12CommandQueue* d3d12CommandQueue() const noexcept { return m_commandQueue.get(); }
    [[nodiscard]] ID3D12Fence* d3d12Fence() const noexcept { return m_event.d3d12Fence(); }
};

} // namespace mini::d3d12