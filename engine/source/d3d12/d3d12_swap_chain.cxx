export module mini.d3d12:swap_chain;

import mini.core;
import mini.graphics;
import mini.windows;
import :descriptor;

namespace mini::d3d12 {

export struct SwapChainBuffer {
    SharedPtr<ID3D12Resource> resource;
    Descriptor descriptor;
    D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc;

    SwapChainBuffer(D3D12_RENDER_TARGET_VIEW_DESC* = nullptr);
};

export class SwapChain final : public graphics::SwapChain {
private:
    static constexpr uint8 MaxBuffer = SwapChain::MaxBackBuffer;

    SharedPtr<IDXGISwapChain3> m_swapChain;
    StaticArray<SwapChainBuffer, MaxBuffer> m_buffers;

    DXGI_SWAP_CHAIN_DESC1 m_swapChainDesc;
    DXGI_SAMPLE_DESC m_sampleDesc;
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC m_fullscreenDesc;
    D3D12_RENDER_TARGET_VIEW_DESC m_rTVDesc;

    uint8 m_index;
    uint8 m_vSync;
    uint32 m_width;
    uint32 m_height;

public:
    SwapChain();

    void ResizeBackBuffer(uint32, uint32, bool) final;
    void SetBackBufferCount(uint8) final;
    void SetVSync(uint8) final;
    void SetFullScreen(bool) final;

    SwapChainBuffer* GetCurrentBuffer() noexcept { return &m_buffers[m_index]; }
    Vector2Int GetBackBufferSize() const final { return Vector2Int(m_width, m_height); }
    uint8 GetBackBufferCount() const final { return (uint8)m_buffers.Size(); }
    uint8 GetVSync() const final { return m_vSync; }
    bool GetFullScreen() const final { return !m_fullscreenDesc.Windowed; }

protected:
    bool Initialize() final;
    void Present() final;

private:
    void ReleaseBuffers();
    void CreateBuffers(uint8);
};

} // namespace mini::d3d12
