module;

#include <Metal/MTL4CommandAllocator.hpp>
#include <Metal/MTL4CommandBuffer.hpp>

#include "option.h"

export module mini.metal4:command_buffer;

import mini.core;
import mini.apple;
import mini.graphics;
import :device;
import :common;
import :command_encoder;

export namespace MTL4 {

using MTL4::CommandAllocator;
using MTL4::CommandAllocatorDescriptor;
using MTL4::CommandBuffer;
using MTL4::CommandBufferOptions;

} // namespace MTL4

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

export class METAL4_API CommandBuffer {
private:
    SharedPtr<MTL4::CommandBuffer> m_commandBuffer;
    SharedPtr<MTL4::CommandBufferOptions> m_commandBufferOptions;
    UniquePtr<LogState> m_logState;

public:
    explicit CommandBuffer(PtrView<Device> device);

    void SetName(StringView name) { m_commandBuffer->setLabel(ToNSString(name).Get()); }
    void Begin(PtrView<CommandAllocator const> allocator);
    void End();

    [[nodiscard]] UniquePtr<RenderCommandEncoder> AllocateRenderCommandEncoder(
        graphics::RenderScopeDescriptor const& descriptor) const;

    [[nodiscard]] bool Valid() const noexcept { return m_commandBuffer.Valid(); }
    [[nodiscard]] String Name() const noexcept { return ToString(m_commandBuffer->label()); }

    [[nodiscard]] MTL4::CommandBuffer* MTLCommandBuffer() const noexcept { return m_commandBuffer.Get(); }

private:
    static void HandleLog(StringView subSystem, StringView category, Logger::Level logLevel, StringView message);
};

class METAL4_API CommandAllocatorPool {
private:
    struct PendingInfo {
        UniquePtr<CommandAllocator> allocator;
        uint64 eventValue;
    };

    Device* m_device;
    Array<UniquePtr<CommandAllocator>> m_pool;
    Array<PendingInfo> m_pending;

public:
    explicit CommandAllocatorPool(PtrView<Device> const& device, size_t poolCapacity = options::bufferCount);

    [[nodiscard]] UniquePtr<CommandAllocator> Allocate();
    void Deallocate(UniquePtr<CommandAllocator>&& allocator);
    void Pending(UniquePtr<CommandAllocator>&& allocator, uint64 eventValue);
    void Expire(uint64 eventValue);
};

} // namespace mini::metal4