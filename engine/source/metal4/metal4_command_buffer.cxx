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

    uint64 AllocatedSize() { return m_commandAllocator->allocatedSize(); }
    void Reset() { m_commandAllocator->reset(); }

    [[nodiscard]] bool Valid() const noexcept { return m_commandAllocator.Valid(); }
    [[nodiscard]] String Name() const noexcept { return ToString(m_commandAllocator->label()); }

    [[nodiscard]] MTL4::CommandAllocator* MTLCommandAllocator() const noexcept { return m_commandAllocator.Get(); }
};

export class METAL4_API CommandBufferOption {
private:
    SharedPtr<MTL4::CommandBufferOptions> m_commandBufferOption;
    LogState m_logState;

public:
    CommandBufferOption() noexcept = default;
    explicit CommandBufferOption(PtrView<Device> device, Logger::Level logLevel);

    [[nodiscard]] bool Valid() const noexcept { return m_commandBufferOption.Valid(); }
    [[nodiscard]] Logger::Level LogLevel() const noexcept { return m_logState.LogLevel(); }

    [[nodiscard]] MTL4::CommandBufferOptions* MTLCommandBufferOption() const noexcept;
    [[nodiscard]] MTL::LogState* MTLLogState() const noexcept { return m_logState.MTLLogState(); }

private:
    static void HandleLog(StringView subSystem, StringView category, Logger::Level logLevel, StringView message);
};

MTL4::CommandBufferOptions* CommandBufferOption::MTLCommandBufferOption() const noexcept
{
    return m_commandBufferOption.Get();
}

export class METAL4_API CommandBuffer {
private:
    SharedPtr<MTL4::CommandBuffer> m_commandBuffer;
    UniquePtr<CommandAllocator> m_commandAllocator;
    PtrView<CommandBufferOption> m_commandBufferOption;

public:
    explicit CommandBuffer(PtrView<Device> device, PtrView<CommandBufferOption> option = nullptr);

    void SetName(StringView name) { m_commandBuffer->setLabel(ToNSString(name).Get()); }
    void Begin(UniquePtr<CommandAllocator> allocator);
    [[nodiscard]] UniquePtr<CommandAllocator> End();

    [[nodiscard]] UniquePtr<RenderCommandEncoder> AllocateRenderCommandEncoder(
        graphics::RenderScopeDescriptor const& descriptor) const;

    [[nodiscard]] bool Valid() const noexcept { return m_commandBuffer.Valid(); }
    [[nodiscard]] bool Active() const noexcept { return m_commandAllocator.Valid(); }
    [[nodiscard]] String Name() const noexcept { return ToString(m_commandBuffer->label()); }

    [[nodiscard]] MTL4::CommandBuffer* MTLCommandBuffer() const noexcept { return m_commandBuffer.Get(); }
    [[nodiscard]] MTL4::CommandBufferOptions* MTLCommandBufferOption() const noexcept;
};

MTL4::CommandBufferOptions* CommandBuffer::MTLCommandBufferOption() const noexcept
{
    return m_commandBufferOption->MTLCommandBufferOption();
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

    [[nodiscard]] UniquePtr<CommandBuffer> Allocate();
    void Allocate(size_t size, Array<UniquePtr<CommandBuffer>>& commandBufferArray);

    void Wait(uint64 commitValue);
    uint64 Commit(UniquePtr<CommandBuffer> commandBuffer);
    uint64 Commit(Array<UniquePtr<CommandBuffer>>& commandBufferArray);

    [[nodiscard]] uint64 SignaledValue() noexcept { return m_sharedEvent.SignaledValue(); }
    [[nodiscard]] uint64 CommittedValue() const noexcept { return m_commitValue; }

private:
    void AllocateCommandBuffer(size_t size);
    void AllocateCommandAllocator(size_t size);
};

} // namespace mini::metal4