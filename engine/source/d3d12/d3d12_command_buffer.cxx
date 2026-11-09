export module mini.d3d12:command_buffer;

import mini.core;
import mini.windows;
import :event;
import :common;

D3D12_EXPORT(ID3D12CommandAllocator);
D3D12_EXPORT(ID3D12CommandList);
D3D12_EXPORT_4(ID3D12GraphicsCommandList);

namespace mini::d3d12 {

class Device;
class CommandQueue;

} // namespace mini::d3d12

namespace mini::d3d12 {

export class D3D12_API CommandAllocator {
private:
    SharedPtr<ID3D12CommandAllocator> m_commandAllocator;

public:
    explicit CommandAllocator(PtrView<Device> device, CommandType commandType);

    void reset() { m_commandAllocator->Reset(); }
    void setName(StringView name) { setDebugName(m_commandAllocator.get(), name); }

    [[nodiscard]] bool valid() const noexcept { return m_commandAllocator.valid(); }
    [[nodiscard]] String name() const noexcept { return debugName(m_commandAllocator.get()); }
    [[nodiscard]] ID3D12CommandAllocator* d3d12CommandAllocator() const noexcept { return m_commandAllocator.get(); }
};

export class D3D12_API CommandBuffer {
private:
    SharedPtr<ID3D12GraphicsCommandList4> m_commandList;
    UniquePtr<CommandAllocator> m_commandAllocator;

public:
    explicit CommandBuffer(PtrView<Device> device);

    void setName(StringView name) { setDebugName(m_commandList.get(), name); }
    void begin(UniquePtr<CommandAllocator> allocator);
    [[nodiscard]] UniquePtr<CommandAllocator> end();

    [[nodiscard]] bool valid() const noexcept { return m_commandList.valid(); }
    [[nodiscard]] bool active() const noexcept { return m_commandAllocator.valid(); }
    [[nodiscard]] String name() const noexcept { return debugName(m_commandList.get()); }

    [[nodiscard]] ID3D12GraphicsCommandList4* d3d12CommandList() const noexcept { return m_commandList.get(); }
};

export class D3D12_API CommandBufferPool {
private:
    struct PendingContext {
        UniquePtr<CommandAllocator> commandAllocator;
        uint64 eventValue;
    };

    PtrView<Device> m_device;
    PtrView<CommandQueue> m_commandQueue;
    CommandType m_commandType;

    SharedEvent m_sharedEvent;
    uint64 m_commitValue;

    Array<UniquePtr<CommandBuffer>> m_commandBufferPool;
    Array<UniquePtr<CommandAllocator>> m_commandAllocatorPool;
    Array<PendingContext> m_pendingBuffer;
    Array<CommandBuffer const*> m_commitBuffer;

public:
    CommandBufferPool(PtrView<Device> device,
                      PtrView<CommandQueue> commandQueue,
                      size_t commandBufferCapacity,
                      size_t commandAllocatorCapacity);

    [[nodiscard]] UniquePtr<CommandBuffer> allocate();
    void allocate(size_t size, Array<UniquePtr<CommandBuffer>>& commandBufferArray);

    void wait(uint64 commitValue);
    uint64 commit(UniquePtr<CommandBuffer> commandBuffer);
    uint64 commit(Array<UniquePtr<CommandBuffer>>& commandBufferArray);

    [[nodiscard]] uint64 signaledValue() noexcept { return m_sharedEvent.signaledValue(); }
    [[nodiscard]] uint64 committedValue() const noexcept { return m_commitValue; }

private:
    void allocateCommandBuffer(size_t size);
    void allocateCommandAllocator(size_t size);
};

} // namespace mini::d3d12