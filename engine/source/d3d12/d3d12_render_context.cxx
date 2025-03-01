module;

#include <d3d12.h>

export module mini.d3d12:render_context;

import mini.core;
import mini.graphics;
import mini.windows;
import :command_queue;
import :swap_chain;

export namespace mini::d3d12
{

class RenderContext : public graphics::RenderContext
{
private:
    CommandQueue* m_CommandQueue;
    SwapChainBuffer* m_CurrentBuffer;

    SharedPtr<ID3D12CommandAllocator> m_CommandAllocator;
    SharedPtr<ID3D12GraphicsCommandList> m_CommandList;

public:
    RenderContext();
    ~RenderContext() final;

    void SetViewport(Rect const&, float, float) final;
    void SetScissorRect(RectInt const&) final;

    void WaitForIdle() final;
    void Execute() final;

    inline CommandQueue* GetCommandQueue() const { return m_CommandQueue; }
    inline ID3D12GraphicsCommandList* GetCommandList() const { return m_CommandList; }

protected:
    bool Initialize() final;

    void BeginRender() final;
    void EndRender() final;
};

} // namespace mini::d3d12