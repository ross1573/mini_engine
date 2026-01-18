module mini.d3d12;

import :command_queue;

namespace mini::d3d12 {

CommandQueue::CommandQueue(ID3D12Device* device, graphics::CommandType queueType)
    : m_commandQueue(nullptr)
    , m_fence(nullptr)
    , m_queueType(queueType)
    , m_queueDesc{}
    , m_fenceHandle(nullptr)
    , m_fenceValue(0)
    , m_lastCompeletedFence(0)
{
    ASSERT(device, "cannot initialize command queue with null device");
    ASSERT(m_queueType != graphics::CommandType::None, "invalid command queue type");

    D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_NONE;
    switch (m_queueType) {
        case graphics::CommandType::Direct:  type = D3D12_COMMAND_LIST_TYPE_DIRECT; break;
        case graphics::CommandType::Copy:    type = D3D12_COMMAND_LIST_TYPE_COPY; break;
        case graphics::CommandType::Compute: type = D3D12_COMMAND_LIST_TYPE_COMPUTE; break;
        default: VERIFY(false, "unknown command queue type");
    }

    m_queueDesc.Type = type;
    m_queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    VERIFY(device->CreateCommandQueue(&m_queueDesc, IID_PPV_ARGS(&m_commandQueue)));
    VERIFY(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

    m_fenceValue = 0;
    m_lastCompeletedFence = 0;
    m_fenceHandle = CreateEventW(nullptr, FALSE, FALSE, L"CommandQueue");
    VERIFY(m_fenceHandle);
}

CommandQueue::~CommandQueue()
{
    m_fence.Reset();

    if (m_fenceHandle) {
        CloseHandle(m_fenceHandle);
        m_fenceHandle = nullptr;
    }
}

void CommandQueue::Wait(CommandQueue* commandQueue)
{
    m_commandQueue->Wait(commandQueue->m_fence, commandQueue->m_fenceValue);
}

void CommandQueue::WaitForFence(uint64 fenceValue)
{
    if (m_lastCompeletedFence > fenceValue) {
        return;
    }

    m_lastCompeletedFence = m_fence->GetCompletedValue();
    if (m_lastCompeletedFence < fenceValue) {
        m_fence->SetEventOnCompletion(fenceValue, m_fenceHandle);
        WaitForSingleObject(m_fenceHandle, INFINITE);
    }
}

void CommandQueue::WaitForIdle()
{
    m_fenceValue++;
    m_commandQueue->Signal(m_fence, m_fenceValue);

    WaitForFence(m_fenceValue);
}

uint64 CommandQueue::ExecuteCommandList(ID3D12CommandList* commandList)
{
    m_fenceValue++;
    m_commandQueue->ExecuteCommandLists(1, &commandList);
    m_commandQueue->Signal(m_fence, m_fenceValue);

    return m_fenceValue;
}

} // namespace mini::d3d12