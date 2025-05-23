module;

#include "assertion.h"

module mini.d3d12;

import :command_queue;

namespace mini::d3d12 {

CommandQueue::CommandQueue(ID3D12Device* device, graphics::CommandType queueType)
    : m_CommandQueue(nullptr)
    , m_Fence(nullptr)
    , m_QueueType(queueType)
    , m_QueueDesc{}
    , m_FenceHandle(nullptr)
    , m_FenceValue(0)
    , m_LastCompeletedFence(0)
{
    ASSERT(device, "cannot initialize command queue with null device");
    ASSERT(m_QueueType != graphics::CommandType::None, "invalid command queue type");

    D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_NONE;
    switch (m_QueueType) {
        case graphics::CommandType::Direct:  type = D3D12_COMMAND_LIST_TYPE_DIRECT; break;
        case graphics::CommandType::Copy:    type = D3D12_COMMAND_LIST_TYPE_COPY; break;
        case graphics::CommandType::Compute: type = D3D12_COMMAND_LIST_TYPE_COMPUTE; break;
        default: VERIFY(false, "unknown command queue type");
    }

    m_QueueDesc.Type = type;
    m_QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    VERIFY(device->CreateCommandQueue(&m_QueueDesc, IID_PPV_ARGS(&m_CommandQueue)));
    VERIFY(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

    m_FenceValue = 0;
    m_LastCompeletedFence = 0;
    m_FenceHandle = CreateEventW(nullptr, FALSE, FALSE, L"CommandQueue");
    VERIFY(m_FenceHandle);
}

CommandQueue::~CommandQueue()
{
    m_Fence.Reset();

    if (m_FenceHandle) {
        CloseHandle(m_FenceHandle);
        m_FenceHandle = nullptr;
    }
}

void CommandQueue::Wait(CommandQueue* commandQueue)
{
    m_CommandQueue->Wait(commandQueue->m_Fence, commandQueue->m_FenceValue);
}

void CommandQueue::WaitForFence(uint64 fenceValue)
{
    if (m_LastCompeletedFence > fenceValue) {
        return;
    }

    m_LastCompeletedFence = m_Fence->GetCompletedValue();
    if (m_LastCompeletedFence < fenceValue) {
        m_Fence->SetEventOnCompletion(fenceValue, m_FenceHandle);
        WaitForSingleObject(m_FenceHandle, INFINITE);
    }
}

void CommandQueue::WaitForIdle()
{
    m_FenceValue++;
    m_CommandQueue->Signal(m_Fence, m_FenceValue);

    WaitForFence(m_FenceValue);
}

uint64 CommandQueue::ExecuteCommandList(ID3D12CommandList* commandList)
{
    m_FenceValue++;
    m_CommandQueue->ExecuteCommandLists(1, &commandList);
    m_CommandQueue->Signal(m_Fence, m_FenceValue);

    return m_FenceValue;
}

} // namespace mini::d3d12