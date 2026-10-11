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
    static constexpr uint8 MaxBuffer = SwapChain::maxFrameBuffer;

    SharedPtr<IDXGISwapChain3> m_swapChain;
    FixedArray<SwapChainBuffer, MaxBuffer> m_buffers;

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

    bool valid() const noexcept final { return m_swapChain.valid(); }
    void present() final;

    void resizeFrameBuffer(uint32, uint32, bool) final;
    void resizeBufferCount(uint8) final;
    void setVSync(uint8) final;
    void setFullScreen(bool) final;

    SwapChainBuffer* GetCurrentBuffer() noexcept { return &m_buffers[m_index]; }
    Vector2Int frameBufferSize() const final { return Vector2Int(m_width, m_height); }
    uint8 bufferCount() const final { return (uint8)m_buffers.size(); }
    uint8 vsync() const final { return m_vSync; }
    bool fullScreen() const final { return !m_fullscreenDesc.Windowed; }

private:
    void ReleaseBuffers();
    void CreateBuffers(uint8);
};

} // namespace mini::d3d12
