export module mini.d3d12:command_queue;

import mini.core;
import mini.graphics;
import mini.windows;

export namespace mini::d3d12 {

class CommandQueue {
private:
    SharedPtr<ID3D12CommandQueue> m_CommandQueue;
    SharedPtr<ID3D12Fence> m_Fence;

    graphics::CommandType m_QueueType;
    D3D12_COMMAND_QUEUE_DESC m_QueueDesc;

    HANDLE m_FenceHandle;
    uint64 m_FenceValue;
    uint64 m_LastCompeletedFence;

public:
    CommandQueue(ID3D12Device*, graphics::CommandType);
    ~CommandQueue();

    void Wait(CommandQueue*);
    void WaitForFence(uint64);
    void WaitForIdle();

    uint64 ExecuteCommandList(ID3D12CommandList*);

    ID3D12CommandQueue* GetD3D12CommandQueue() const { return m_CommandQueue; }
    HANDLE GetFenceHandle() const { return m_FenceHandle; }
    uint64 GetCurrentFence() const { return m_FenceValue; }
};

} // namespace mini::d3d12