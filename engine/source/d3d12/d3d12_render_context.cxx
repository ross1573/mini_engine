export module mini.d3d12:render_context;

import mini.core;
import mini.graphics;
import mini.windows;
import :command_queue;
import :swap_chain;

export namespace mini::d3d12 {

class RenderContext final : public graphics::RenderContext {
private:
    SharedPtr<ID3D12Device> m_device;
    UniquePtr<CommandQueue> m_commandQueue;
    SwapChainBuffer* m_currentBuffer;

    SharedPtr<ID3D12CommandAllocator> m_commandAllocator;
    SharedPtr<ID3D12GraphicsCommandList> m_commandList;

public:
    RenderContext(ID3D12Device*);
    ~RenderContext();

    bool Initialize() final;

    void BeginRender() final;
    void EndRender() final;
    void WaitForIdle() final;
    void Execute() final;

    void SetViewport(Rect const&, float32, float32) final;
    void SetScissorRect(RectInt const&) final;

    CommandQueue* GetCommandQueue() const { return m_commandQueue.Get(); }
    ID3D12GraphicsCommandList* GetCommandList() const { return m_commandList; }
};

} // namespace mini::d3d12