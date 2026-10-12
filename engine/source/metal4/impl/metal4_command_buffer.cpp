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

    m_commandAllocator = transfer(device->mtlDevice()->newCommandAllocator());
}

CommandAllocator::CommandAllocator(PtrView<Device> device, StringView name)
{
    ASSERT(device);

    SharedPtr<NS::String> label = toNSString(name);
    MTL4::CommandAllocatorDescriptor* desc = MTL4::CommandAllocatorDescriptor::alloc();
    desc->init();
    desc->setLabel(label.get());

    NS::Error* error;
    m_commandAllocator = transfer(device->mtlDevice()->newCommandAllocator(desc, &error));
    ENSURE(m_commandAllocator, error, "failed to create MTL4::CommandAllocator") {
        return;
    }
}

CommandBufferOption::CommandBufferOption(PtrView<Device> device, Logger::Level level)
    : m_logState(device, level, &CommandBufferOption::handleLog)
{
    m_commandBufferOption = transfer(MTL4::CommandBufferOptions::alloc());
    m_commandBufferOption->init();
    m_commandBufferOption->setLogState(m_logState.mtlLogState());
}

void CommandBufferOption::handleLog(StringView subSystem, StringView category, Logger::Level level, StringView message)
{
    log(level,
        "\n\nMetal validation failed\n"
        "- category: {} ({})"
        "- message: {}",
        category,
        subSystem,
        message);

    ENSURE(level < Logger::Level::warn) { }
}

CommandBuffer::CommandBuffer(PtrView<Device> device, PtrView<CommandBufferOption> option)
{
    ASSERT(device);

    m_commandBuffer = transfer(device->mtlDevice()->newCommandBuffer());
    m_commandBufferOption = option;
}

void CommandBuffer::begin(UniquePtr<CommandAllocator> allocator)
{
    ASSERT(allocator.valid());
    ENSURE(!m_commandAllocator.valid(),
           "previous command allocator not released."
           "please submit command buffer before assigning new command allocator") {
        return;
    }

    m_commandAllocator = moveCast(allocator);

    if (m_commandBufferOption.valid()) {
        m_commandBuffer->beginCommandBuffer(m_commandAllocator->mtlCommandAllocator(),
                                            m_commandBufferOption->mtlCommandBufferOption());
    } else {
        m_commandBuffer->beginCommandBuffer(m_commandAllocator->mtlCommandAllocator());
    }
}

[[nodiscard]] UniquePtr<CommandAllocator> CommandBuffer::end()
{
    m_commandBuffer->endCommandBuffer();
    return moveCast(m_commandAllocator);
}

UniquePtr<RenderCommandEncoder> CommandBuffer::renderCommandEncoder(
    graphics::RenderScopeDescriptor const& descriptor) const
{
    ASSERT(valid() && active());

    SharedPtr<MTL4::RenderPassDescriptor> desc = mtlRenderPassDescriptor(descriptor);
    MTL4::RenderCommandEncoder* mtlRenderCommandEncoder = mtlCommandBuffer()->renderCommandEncoder(desc.get());
    ENSURE(mtlRenderCommandEncoder, "failed to create renderpass") {
        return nullptr;
    }

    return makeUnique<RenderCommandEncoder>(mtlRenderCommandEncoder);
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
        Logger::Level level = RELEASE ? Logger::Level::warn : Logger::Level::debug;
        m_commandBufferOption = CommandBufferOption(device, level);
    }

    allocateCommandBuffer(commandBufferCapacity);
    allocateCommandAllocator(commandAllocatorCapacity);
}

void CommandBufferAllocator::allocateCommandBuffer(size_t size)
{
    CommandBufferOption* commandBufferOption =
        m_commandBufferOption.valid() ? addressof(m_commandBufferOption) : nullptr;

    m_commandBufferPool.reserve(m_commandBufferPool.size() + size);
    for (size_t i = 0; i < size; ++i) {
        m_commandBufferPool.pushBack(makeUnique<CommandBuffer>(m_device, commandBufferOption));
    }
}

void CommandBufferAllocator::allocateCommandAllocator(size_t size)
{
    m_commandAllocatorPool.reserve(m_commandAllocatorPool.size() + size);
    for (size_t i = 0; i < size; ++i) {
        m_commandAllocatorPool.pushBack(makeUnique<CommandAllocator>(m_device));
    }
}

UniquePtr<CommandBuffer> CommandBufferAllocator::allocate()
{
    if (m_commandBufferPool.empty()) {
        allocateCommandBuffer(1);
    }

    if (m_commandAllocatorPool.empty()) {
        allocateCommandAllocator(1);
    }

    UniquePtr<CommandBuffer> commandBuffer = m_commandBufferPool.popLast();
    UniquePtr<CommandAllocator> commandAllocator = m_commandAllocatorPool.popLast();

    commandBuffer->begin(moveCast(commandAllocator));
    return moveCast(commandBuffer);
}

void CommandBufferAllocator::allocate(size_t size, Array<UniquePtr<CommandBuffer>>& commandBufferArray)
{
    offset_t commandBufferAllocateSize = static_cast<offset_t>(size - m_commandBufferPool.size());
    if (commandBufferAllocateSize > 0) {
        allocateCommandBuffer(static_cast<size_t>(commandBufferAllocateSize));
    }

    offset_t commandAllocatorAllocateSize = static_cast<offset_t>(size - m_commandAllocatorPool.size());
    if (commandAllocatorAllocateSize > 0) {
        allocateCommandAllocator(static_cast<size_t>(commandAllocatorAllocateSize));
    }

    commandBufferArray.reserve(commandBufferArray.size() + size);
    for (size_t i = 0; i < size; ++i) {
        UniquePtr<CommandBuffer> commandBuffer = m_commandBufferPool.popLast();
        commandBuffer->begin(m_commandAllocatorPool.popLast());
        commandBufferArray.pushBack(moveCast(commandBuffer));
    }
}

void CommandBufferAllocator::wait(uint64 commitValue)
{
    m_sharedEvent.wait(commitValue);

    Array<PendingContext>::Iterator iterator = m_pendingBuffer.begin();
    for (; iterator != m_pendingBuffer.end();) {
        if (iterator->eventValue > commitValue) {
            ++iterator;
            continue;
        }

        iterator->commandAllocator->reset();
        m_commandAllocatorPool.pushBack(moveCast(iterator->commandAllocator));
        m_pendingBuffer.remove(iterator);
    }
}

uint64 CommandBufferAllocator::commit(UniquePtr<CommandBuffer> commandBuffer)
{
    ENSURE(commandBuffer, "commit attempt on invalid command buffer") {
        return m_commitValue;
    }

    m_pendingBuffer.pushBack(PendingContext{
        .commandAllocator = commandBuffer->end(),
        .eventValue = ++m_commitValue,
    });

    PtrView<CommandBuffer> commandBufferPtr = commandBuffer;
    m_commandBufferPool.pushBack(moveCast(commandBuffer));
    m_commandQueue->commit(commandBufferPtr);
    m_commandQueue->signal(addressof(m_sharedEvent), m_commitValue);
    return m_commitValue;
}

uint64 CommandBufferAllocator::commit(Array<UniquePtr<CommandBuffer>>& commandBufferArray)
{
    size_t commitValue = m_commitValue + 1;
    m_commitBuffer.reserve(commandBufferArray.size());

    for (UniquePtr<CommandBuffer>& commandBuffer : commandBufferArray) {
        ENSURE(commandBuffer, "commit attempt on invalid command buffer") {
            continue;
        }

        m_pendingBuffer.pushBack(PendingContext{
            .commandAllocator = commandBuffer->end(),
            .eventValue = commitValue,
        });

        m_commitBuffer.pushBack(commandBuffer.get());
        m_commandBufferPool.pushBack(moveCast(commandBuffer));
    }

    commandBufferArray.clear();
    if (m_commitBuffer.empty()) {
        return m_commitValue;
    }

    m_commandQueue->commit(m_commitBuffer);
    m_commandQueue->signal(addressof(m_sharedEvent), ++m_commitValue);
    m_commitBuffer.clear();
    return m_commitValue;
}

} // namespace mini::metal4