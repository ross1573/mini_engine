module mini.d3d12;

import :device;
import :command_buffer;

namespace mini::d3d12 {

CommandAllocator::CommandAllocator(PtrView<Device> device, CommandType commandType)
{
    ASSERT(device);

    SharedPtr<ID3D12CommandAllocator> commandAllocator;
    D3D12_COMMAND_LIST_TYPE type = d3d12CommandType(commandType);

    ENSURE(device->d3d12Device()->CreateCommandAllocator(type, COM_ARG(&commandAllocator)),
           "failed to create command allocator") {
        return;
    }

    m_commandAllocator = moveCast(commandAllocator);
}

CommandBuffer::CommandBuffer(PtrView<Device> device)
{
    ASSERT(device);

    SharedPtr<ID3D12GraphicsCommandList4> commandList;
    D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    D3D12_COMMAND_LIST_FLAGS flag = D3D12_COMMAND_LIST_FLAG_NONE;

    ENSURE(device->d3d12Device()->CreateCommandList1(0, type, flag, COM_ARG(&commandList)),
           "failed to create commmand list") {
        return;
    }

    m_commandList = moveCast(commandList);
}

void CommandBuffer::begin(UniquePtr<CommandAllocator> allocator)
{
    ASSERT(allocator.valid());
    ENSURE(!m_commandAllocator.valid(),
           "previous command allocator not released."
           "please submit command list before assigning new command allocator") {
        return;
    }

    m_commandAllocator = moveCast(allocator);
    HRESULT resetResult = m_commandList->Reset(m_commandAllocator->d3d12CommandAllocator(), nullptr);
    ENSURE(resetResult, "failed to reset ID3D12CommandList. code: {}", static_cast<int32>(resetResult)) { }
}

UniquePtr<CommandAllocator> CommandBuffer::end()
{
    ENSURE(active(), "trying to end command list which hasn't been started") {
        return nullptr;
    }

    HRESULT closeResult = m_commandList->Close();
    ENSURE(closeResult, "failed to close ID3D12CommandList. code: {}", static_cast<int32>(closeResult)) { }
    return moveCast(m_commandAllocator);
}

CommandBufferPool::CommandBufferPool(PtrView<Device> device,
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

    D3D12_COMMAND_QUEUE_DESC desc;
#if PLATFORM_MINGW
    ignore = commandQueue->d3d12CommandQueue()->GetDesc(&desc);
#else
    desc = commandQueue->d3d12CommandQueue()->GetDesc();
#endif
    m_commandType = commandType(desc.Type);

    allocateCommandBuffer(commandBufferCapacity);
    allocateCommandAllocator(commandAllocatorCapacity);
}

void CommandBufferPool::allocateCommandBuffer(size_t size)
{
    m_commandBufferPool.reserve(m_commandBufferPool.size() + size);
    for (size_t i = 0; i < size; ++i) {
        m_commandBufferPool.pushBack(makeUnique<CommandBuffer>(m_device));
    }
}

void CommandBufferPool::allocateCommandAllocator(size_t size)
{
    m_commandAllocatorPool.reserve(m_commandAllocatorPool.size() + size);
    for (size_t i = 0; i < size; ++i) {
        m_commandAllocatorPool.pushBack(makeUnique<CommandAllocator>(m_device, CommandType::direct));
    }
}

UniquePtr<CommandBuffer> CommandBufferPool::allocate()
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
    return commandBuffer;
}

void CommandBufferPool::allocate(size_t size, Array<UniquePtr<CommandBuffer>>& commandBufferArray)
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

void CommandBufferPool::wait(uint64 commitValue)
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

uint64 CommandBufferPool::commit(UniquePtr<CommandBuffer> commandBuffer)
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

uint64 CommandBufferPool::commit(Array<UniquePtr<CommandBuffer>>& commandBufferArray)
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

} // namespace mini::d3d12