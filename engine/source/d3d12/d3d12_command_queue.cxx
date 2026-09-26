export module mini.d3d12:command_queue;

import mini.core;
import mini.graphics;
import mini.windows;

export namespace mini::d3d12 {

export enum class CommandType : int8 {
    None = -1,

    Direct,
    Compute,
    Copy,
};

class CommandQueue {
private:
    SharedPtr<ID3D12CommandQueue> m_commandQueue;
    SharedPtr<ID3D12Fence> m_fence;

    CommandType m_queueType;
    D3D12_COMMAND_QUEUE_DESC m_queueDesc;

    HANDLE m_fenceHandle;
    uint64 m_fenceValue;
    uint64 m_lastCompeletedFence;

public:
    CommandQueue(ID3D12Device*, CommandType);
    ~CommandQueue();

    void Wait(CommandQueue*);
    void WaitForFence(uint64);
    void WaitForIdle();

    uint64 ExecuteCommandList(ID3D12CommandList*);

    ID3D12CommandQueue* GetD3D12CommandQueue() const { return m_commandQueue; }
    HANDLE GetFenceHandle() const { return m_fenceHandle; }
    uint64 GetCurrentFence() const { return m_fenceValue; }
};

} // namespace mini::d3d12