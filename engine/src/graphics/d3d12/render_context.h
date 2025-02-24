#pragma once

#include "graphics/render_context.h"

namespace mini::d3d12
{
class CommandQueue;
struct SwapChainBuffer;
}

namespace mini::d3d12
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

    bool Initialize() final;

    void BeginRender() final;
    void EndRender() final;

    void SetViewport(Rect const&, float, float) final;
    void SetScissorRect(RectInt const&) final;

    void WaitForIdle() final;
    void Execute() final;

    CommandQueue* GetCommandQueue() const { return m_CommandQueue; }
    ID3D12GraphicsCommandList* GetCommandList() const { return m_CommandList; }
};

} // namespace mini::d3d12