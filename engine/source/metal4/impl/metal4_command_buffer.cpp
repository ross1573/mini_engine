module mini.metal4;

import :command_buffer;
import :log;

namespace mini::metal4 {

CommandAllocator::CommandAllocator(PtrView<Device> device)
{
    ASSERT(device);

    m_commandAllocator = TransferShared(device->MTLDevice()->newCommandAllocator());
}

CommandAllocator::CommandAllocator(PtrView<Device> device, StringView name)
{
    ASSERT(device);

    MTL4::CommandAllocatorDescriptor* desc = MTL4::CommandAllocatorDescriptor::alloc();
    ENSURE(desc != nullptr) {
        return;
    }

    SharedPtr<NS::String> label = ToNSString(name);
    desc->init();
    desc->setLabel(label.Get());

    NS::Error* error;
    m_commandAllocator = TransferShared(device->MTLDevice()->newCommandAllocator(desc, &error));
    ENSURE(m_commandAllocator, error, "failed to create MTL4::CommandAllocator") {
        return;
    }
}

CommandBuffer::CommandBuffer(PtrView<Device> device)
{
    ASSERT(device);

    m_commandBuffer = TransferShared(device->MTLDevice()->newCommandBuffer());

    if (options::gpuValidation) {
        Logger::Level logLevel = RELEASE ? Logger::Level::warn : Logger::Level::debug;
        m_logState = MakeUnique<LogState>(device, logLevel, &CommandBuffer::HandleLog);
        m_commandBufferOptions = TransferShared(MTL4::CommandBufferOptions::alloc());
        m_commandBufferOptions->init();
        m_commandBufferOptions->setLogState(m_logState->MTLLogState());
    }
}

void CommandBuffer::Begin(PtrView<CommandAllocator const> allocator)
{
    ASSERT(allocator);

    if (m_logState.Valid()) {
        m_commandBuffer->beginCommandBuffer(allocator->MTLCommandAllocator(), m_commandBufferOptions.Get());
    } else {
        m_commandBuffer->beginCommandBuffer(allocator->MTLCommandAllocator());
    }
}

void CommandBuffer::End()
{
    m_commandBuffer->endCommandBuffer();
}

void CommandBuffer::HandleLog(StringView subSystem, StringView category, Logger::Level logLevel, StringView message)
{
    Log(logLevel,
        "\n\nMetal validation failed\n"
        "- category: {} ({})"
        "- message: {}",
        category,
        subSystem,
        message);

    ENSURE(logLevel < Logger::Level::warn) { }
}

CommandAllocatorPool::CommandAllocatorPool(PtrView<Device> const& device, size_t poolCapacity)
    : m_device(device)
    , m_pool(poolCapacity)
    , m_pending(poolCapacity)
{
    ASSERT(m_device);
}

UniquePtr<CommandAllocator> CommandAllocatorPool::Allocate()
{
    if (m_pool.Empty()) {
        return MakeUnique<CommandAllocator>(m_device);
    }

    UniquePtr<CommandAllocator> alloc = m_pool.PopLast();
    return alloc;
}

void CommandAllocatorPool::Deallocate(UniquePtr<CommandAllocator>&& allocator)
{
    allocator->Reset();
    m_pool.PushBack(MoveArg(allocator));
}

void CommandAllocatorPool::Pending(UniquePtr<CommandAllocator>&& allocator, uint64 eventValue)
{
    m_pending.PushBack(MoveArg(allocator), eventValue);
}

void CommandAllocatorPool::Expire(uint64 eventValue)
{
    Array<PendingInfo>::Iterator iterator = m_pending.Begin();
    for (; iterator != m_pending.End();) {
        if (iterator->eventValue <= eventValue) {
            ++iterator;
            continue;
        }

        Deallocate(MoveArg(iterator->allocator));
        m_pending.Remove(iterator);
    }
}

} // namespace mini::metal4