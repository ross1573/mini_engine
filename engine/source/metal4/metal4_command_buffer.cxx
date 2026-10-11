module;

#include <Metal/MTL4CommandAllocator.hpp>
#include <Metal/MTL4CommandBuffer.hpp>

export module mini.metal4:command_buffer;

import mini.core;
import mini.apple;
import mini.graphics;
import :common;
import :event;

export namespace MTL4 {

using MTL4::CommandAllocator;
using MTL4::CommandAllocatorDescriptor;
using MTL4::CommandBuffer;
using MTL4::CommandBufferOptions;

} // namespace MTL4

namespace mini::metal4 {

class Device;
class CommandQueue;
class RenderCommandEncoder;

} // namespace mini::metal4

namespace mini::metal4 {

export class METAL4_API CommandAllocator {
private:
    SharedPtr<MTL4::CommandAllocator> m_commandAllocator;

public:
    explicit CommandAllocator(PtrView<Device> device);
    CommandAllocator(PtrView<Device> device, StringView name);

    uint64 allocatedSize() { return m_commandAllocator->allocatedSize(); }
    void reset() { m_commandAllocator->reset(); }

    [[nodiscard]] bool valid() const noexcept { return m_commandAllocator.valid(); }
    [[nodiscard]] String name() const noexcept { return toString(m_commandAllocator->label()); }

    [[nodiscard]] MTL4::CommandAllocator* mtlCommandAllocator() const noexcept { return m_commandAllocator.get(); }
};

export class METAL4_API CommandBufferOption {
private:
    SharedPtr<MTL4::CommandBufferOptions> m_commandBufferOption;
    LogState m_logState;

public:
    CommandBufferOption() noexcept = default;
    explicit CommandBufferOption(PtrView<Device> device, Logger::Level level);

    [[nodiscard]] bool valid() const noexcept { return m_commandBufferOption.valid(); }
    [[nodiscard]] Logger::Level logLevel() const noexcept { return m_logState.logLevel(); }

    [[nodiscard]] MTL4::CommandBufferOptions* mtlCommandBufferOption() const noexcept;
    [[nodiscard]] MTL::LogState* mtlLogState() const noexcept { return m_logState.mtlLogState(); }

private:
    static void handleLog(StringView subSystem, StringView category, Logger::Level logLevel, StringView message);
};

MTL4::CommandBufferOptions* CommandBufferOption::mtlCommandBufferOption() const noexcept
{
    return m_commandBufferOption.get();
}

export class METAL4_API CommandBuffer {
private:
    SharedPtr<MTL4::CommandBuffer> m_commandBuffer;
    UniquePtr<CommandAllocator> m_commandAllocator;
    PtrView<CommandBufferOption> m_commandBufferOption;

public:
    explicit CommandBuffer(PtrView<Device> device, PtrView<CommandBufferOption> option = nullptr);

    void setName(StringView name) { m_commandBuffer->setLabel(toNSString(name).get()); }
    void begin(UniquePtr<CommandAllocator> allocator);
    [[nodiscard]] UniquePtr<CommandAllocator> end();

    [[nodiscard]] UniquePtr<RenderCommandEncoder> renderCommandEncoder(
        graphics::RenderScopeDescriptor const& descriptor) const;

    [[nodiscard]] bool valid() const noexcept { return m_commandBuffer.valid(); }
    [[nodiscard]] bool active() const noexcept { return m_commandAllocator.valid(); }
    [[nodiscard]] String name() const noexcept { return toString(m_commandBuffer->label()); }

    [[nodiscard]] MTL4::CommandBuffer* mtlCommandBuffer() const noexcept { return m_commandBuffer.get(); }
    [[nodiscard]] MTL4::CommandBufferOptions* mtlCommandBufferOption() const noexcept;
};

MTL4::CommandBufferOptions* CommandBuffer::mtlCommandBufferOption() const noexcept
{
    return m_commandBufferOption->mtlCommandBufferOption();
}

export class METAL4_API CommandBufferAllocator {
private:
    struct PendingContext {
        UniquePtr<CommandAllocator> commandAllocator;
        uint64 eventValue;
    };

    PtrView<Device> m_device;
    PtrView<CommandQueue> m_commandQueue;
    CommandBufferOption m_commandBufferOption;

    SharedEvent m_sharedEvent;
    uint64 m_commitValue;

    Array<UniquePtr<CommandBuffer>> m_commandBufferPool;
    Array<UniquePtr<CommandAllocator>> m_commandAllocatorPool;
    Array<PendingContext> m_pendingBuffer;
    Array<CommandBuffer const*> m_commitBuffer;

public:
    CommandBufferAllocator(PtrView<Device> device,
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

} // namespace mini::metal4