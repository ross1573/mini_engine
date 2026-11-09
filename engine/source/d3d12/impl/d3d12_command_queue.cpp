module mini.d3d12;

import :command_buffer;
import :command_queue;

namespace mini::d3d12 {

CommandQueue::CommandQueue(PtrView<Device> device, CommandType commandType)
    : m_commandQueue(nullptr)
    , m_event(device)
{
    ASSERT(device, "cannot initialize command queue with null device");

    D3D12_COMMAND_QUEUE_DESC desc{
        .Type = d3d12CommandType(commandType),
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 0,
    };

    VERIFY(device->d3d12Device()->CreateCommandQueue(&desc, COM_ARG(&m_commandQueue)));
}

void CommandQueue::commit(PtrView<CommandBuffer const> commandBuffer)
{
    ASSERT(commandBuffer);

    ID3D12CommandList* buffer[] = {commandBuffer->d3d12CommandList()};
    m_commandQueue->ExecuteCommandLists(1, buffer);
    m_commandQueue->Signal(d3d12Fence(), ++m_eventValue);
}

void CommandQueue::commit(ArrayView<CommandBuffer const*> commandBuffers)
{
    if (commandBuffers.empty()) [[unlikely]] {
        return;
    }

    m_commitBuffer.reserve(commandBuffers.size());
    for (auto const& cmdBuf : commandBuffers) {
        ASSERT(cmdBuf);
        m_commitBuffer.pushBack(cmdBuf->d3d12CommandList());
    }

    UINT commitSize = static_cast<UINT>(m_commitBuffer.size());
    m_commandQueue->ExecuteCommandLists(commitSize, m_commitBuffer.data());
    m_commandQueue->Signal(d3d12Fence(), ++m_eventValue);
}

void CommandQueue::wait(PtrView<CommandQueue const> commandQueue)
{
    ASSERT(commandQueue);

    m_commandQueue->Wait(commandQueue->d3d12Fence(), commandQueue->m_eventValue);
}

void CommandQueue::signal(PtrView<Event const> event, uint64 value)
{
    ASSERT(event);

    m_commandQueue->Signal(event->d3d12Fence(), value);
}

} // namespace mini::d3d12