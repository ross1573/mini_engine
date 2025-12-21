export module mini.d3d12:render_context;

import mini.type;
import mini.graphics;
import mini.windows;
import :command_queue;
import :swap_chain;

export namespace mini::d3d12 {

class RenderContext final : public graphics::RenderContext {
private:
    UniquePtr<CommandQueue> m_CommandQueue;
    SwapChainBuffer* m_CurrentBuffer;

    SharedPtr<ID3D12CommandAllocator> m_CommandAllocator;
    SharedPtr<ID3D12GraphicsCommandList> m_CommandList;

public:
    RenderContext();
    ~RenderContext() final;

    bool Initialize() final;

    void BeginRender() final;
    void EndRender() final;
    void WaitForIdle() final;
    void Execute() final;

    void SetViewport(Rect const&, float32, float32) final;
    void SetScissorRect(RectInt const&) final;

    inline CommandQueue* GetCommandQueue() const { return m_CommandQueue.Get(); }
    inline ID3D12GraphicsCommandList* GetCommandList() const { return m_CommandList; }
};

} // namespace mini::d3d12