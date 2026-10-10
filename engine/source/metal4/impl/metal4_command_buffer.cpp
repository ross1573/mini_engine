module mini.metal4;

import :device;
import :command_encoder;
import :command_buffer;
import :command_queue;
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

    SharedPtr<NS::String> label = ToNSString(name);
    MTL4::CommandAllocatorDescriptor* desc = MTL4::CommandAllocatorDescriptor::alloc();
    desc->init();
    desc->setLabel(label.Get());

    NS::Error* error;
    m_commandAllocator = TransferShared(device->MTLDevice()->newCommandAllocator(desc, &error));
    ENSURE(m_commandAllocator, error, "failed to create MTL4::CommandAllocator") {
        return;
    }
}

CommandBufferOption::CommandBufferOption(PtrView<Device> device, Logger::Level logLevel)
    : m_logState(device, logLevel, &CommandBufferOption::HandleLog)
{
    m_commandBufferOption = TransferShared(MTL4::CommandBufferOptions::alloc());
    m_commandBufferOption->init();
    m_commandBufferOption->setLogState(m_logState.MTLLogState());
}

void CommandBufferOption::HandleLog(StringView subSystem,
                                    StringView category,
                                    Logger::Level logLevel,
                                    StringView message)
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

CommandBuffer::CommandBuffer(PtrView<Device> device, PtrView<CommandBufferOption> option)
{
    ASSERT(device);

    m_commandBuffer = TransferShared(device->MTLDevice()->newCommandBuffer());
    m_commandBufferOption = option;
}

void CommandBuffer::Begin(UniquePtr<CommandAllocator> allocator)
{
    ASSERT(allocator.Valid());
    ENSURE(!m_commandAllocator.Valid(),
           "previous command allocator not released."
           "please submit command buffer before assigning new command allocator") {
        return;
    }

    m_commandAllocator = MoveArg(allocator);

    if (m_commandBufferOption.Valid()) {
        m_commandBuffer->beginCommandBuffer(m_commandAllocator->MTLCommandAllocator(),
                                            m_commandBufferOption->MTLCommandBufferOption());
    } else {
        m_commandBuffer->beginCommandBuffer(m_commandAllocator->MTLCommandAllocator());
    }
}

[[nodiscard]] UniquePtr<CommandAllocator> CommandBuffer::End()
{
    m_commandBuffer->endCommandBuffer();
    return MoveArg(m_commandAllocator);
}

UniquePtr<RenderCommandEncoder> CommandBuffer::AllocateRenderCommandEncoder(
    graphics::RenderScopeDescriptor const& descriptor) const
{
    ASSERT(Valid() && Active());

    SharedPtr<MTL4::RenderPassDescriptor> desc = MTLRenderPassDescriptor(descriptor);
    MTL4::RenderCommandEncoder* mtlRenderCommandEncoder = MTLCommandBuffer()->renderCommandEncoder(desc.Get());
    ENSURE(mtlRenderCommandEncoder, "failed to create renderpass") {
        return nullptr;
    }

    return MakeUnique<RenderCommandEncoder>(mtlRenderCommandEncoder);
}

CommandBufferAllocator::CommandBufferAllocator(PtrView<Device> device,
                                               PtrView<CommandQueue> commandQueue,
                                               size_t commandBufferCapacity,
                                               size_t commandAllocatorCapacity)
    : m_device(device)
    , m_commandQueue(commandQueue)
    , m_sharedEvent(device)
    , m_commitValue(0)
    , m_commandBufferPool(commandBufferCapacity)
    , m_commandAllocatorPool(commandAllocatorCapacity)
    , m_pendingBuffer(commandAllocatorCapacity)
    , m_commitBuffer(commandAllocatorCapacity)
{
    ASSERT(m_device);
    ASSERT(m_commandQueue);

    if (options::gpuValidation) {
        Logger::Level logLevel = RELEASE ? Logger::Level::warn : Logger::Level::debug;
        m_commandBufferOption = CommandBufferOption(device, logLevel);
    }

    AllocateCommandBuffer(commandBufferCapacity);
    AllocateCommandAllocator(commandAllocatorCapacity);
}

void CommandBufferAllocator::AllocateCommandBuffer(size_t size)
{
    CommandBufferOption* commandBufferOption =
        m_commandBufferOption.Valid() ? memory::AddressOf(m_commandBufferOption) : nullptr;

    m_commandBufferPool.Reserve(m_commandBufferPool.Size() + size);
    for (size_t i = 0; i < size; ++i) {
        m_commandBufferPool.PushBack(MakeUnique<CommandBuffer>(m_device, commandBufferOption));
    }
}

void CommandBufferAllocator::AllocateCommandAllocator(size_t size)
{
    m_commandAllocatorPool.Reserve(m_commandAllocatorPool.Size() + size);
    for (size_t i = 0; i < size; ++i) {
        m_commandAllocatorPool.PushBack(MakeUnique<CommandAllocator>(m_device));
    }
}

UniquePtr<CommandBuffer> CommandBufferAllocator::Allocate()
{
    if (m_commandBufferPool.Empty()) {
        AllocateCommandBuffer(1);
    }

    if (m_commandAllocatorPool.Empty()) {
        AllocateCommandAllocator(1);
    }

    UniquePtr<CommandBuffer> commandBuffer = m_commandBufferPool.PopLast();
    UniquePtr<CommandAllocator> commandAllocator = m_commandAllocatorPool.PopLast();

    commandBuffer->Begin(MoveArg(commandAllocator));
    return MoveArg(commandBuffer);
}

void CommandBufferAllocator::Allocate(size_t size, Array<UniquePtr<CommandBuffer>>& commandBufferArray)
{
    offset_t commandBufferAllocateSize = static_cast<offset_t>(size - m_commandBufferPool.Size());
    if (commandBufferAllocateSize > 0) {
        AllocateCommandBuffer(static_cast<size_t>(commandBufferAllocateSize));
    }

    offset_t commandAllocatorAllocateSize = static_cast<offset_t>(size - m_commandAllocatorPool.Size());
    if (commandAllocatorAllocateSize > 0) {
        AllocateCommandAllocator(static_cast<size_t>(commandAllocatorAllocateSize));
    }

    commandBufferArray.Reserve(commandBufferArray.Size() + size);
    for (size_t i = 0; i < size; ++i) {
        UniquePtr<CommandBuffer> commandBuffer = m_commandBufferPool.PopLast();
        commandBuffer->Begin(m_commandAllocatorPool.PopLast());
        commandBufferArray.PushBack(MoveArg(commandBuffer));
    }
}

void CommandBufferAllocator::Wait(uint64 commitValue)
{
    m_sharedEvent.Wait(commitValue);

    Array<PendingContext>::Iterator iterator = m_pendingBuffer.Begin();
    for (; iterator != m_pendingBuffer.End();) {
        if (iterator->eventValue > commitValue) {
            ++iterator;
            continue;
        }

        iterator->commandAllocator->Reset();
        m_commandAllocatorPool.PushBack(MoveArg(iterator->commandAllocator));
        m_pendingBuffer.Remove(iterator);
    }
}

uint64 CommandBufferAllocator::Commit(UniquePtr<CommandBuffer> commandBuffer)
{
    ENSURE(commandBuffer, "commit attempt on invalid command buffer") {
        return m_commitValue;
    }

    m_pendingBuffer.PushBack(PendingContext{
        .commandAllocator = commandBuffer->End(),
        .eventValue = ++m_commitValue,
    });

    PtrView<CommandBuffer> commandBufferPtr = commandBuffer;
    m_commandBufferPool.PushBack(MoveArg(commandBuffer));
    m_commandQueue->Commit(commandBufferPtr);
    m_commandQueue->Signal(memory::AddressOf(m_sharedEvent), m_commitValue);
    return m_commitValue;
}

uint64 CommandBufferAllocator::Commit(Array<UniquePtr<CommandBuffer>>& commandBufferArray)
{
    size_t commitValue = m_commitValue + 1;
    m_commitBuffer.Reserve(commandBufferArray.Size());

    for (UniquePtr<CommandBuffer>& commandBuffer : commandBufferArray) {
        ENSURE(commandBuffer, "commit attempt on invalid command buffer") {
            continue;
        }

        m_pendingBuffer.PushBack(PendingContext{
            .commandAllocator = commandBuffer->End(),
            .eventValue = commitValue,
        });

        m_commitBuffer.PushBack(commandBuffer.Get());
        m_commandBufferPool.PushBack(MoveArg(commandBuffer));
    }

    commandBufferArray.Clear();
    if (m_commitBuffer.Empty()) {
        return m_commitValue;
    }

    m_commandQueue->Commit(m_commitBuffer);
    m_commandQueue->Signal(memory::AddressOf(m_sharedEvent), ++m_commitValue);
    m_commitBuffer.Clear();
    return m_commitValue;
}

} // namespace mini::metal4