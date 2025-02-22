#pragma once

#include "core/utility.h"
#include "graphics/graphics.h"
#include "platform/windows/shared_ptr.h"

namespace mini::d3d12
{

class CommandQueue
{
private:
    SharedPtr<ID3D12Device> m_Device;
    SharedPtr<ID3D12CommandQueue> m_CommandQueue;

    graphics::CommandType m_QueueType;
    D3D12_COMMAND_QUEUE_DESC m_QueueDesc;

    SharedPtr<ID3D12Fence> m_Fence;
    HANDLE m_FenceHandle;
    uint64 m_FenceValue;
    uint64 m_LastCompeletedFence;

public:
    CommandQueue(ID3D12Device*, graphics::CommandType);
    ~CommandQueue();

    bool Initialize();

    void Wait(CommandQueue*);
    void WaitForFence(uint64);
    void WaitForIdle();

    uint64 IncrementFence();
    uint64 ExecuteCommandList(ID3D12CommandList*);

    ID3D12CommandQueue* GetD3D12CommandQueue() const { return m_CommandQueue.Get(); }
    HANDLE GetFenceHandle() const { return m_FenceHandle; }
    uint64 GetCurrentFence() const { return m_FenceValue; }
};

} // namespace mini::d3d12