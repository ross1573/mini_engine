#pragma once

#include "core/utility.h"
#include "container/static_array.h"
#include "graphics/swap_chain.h"
#include "graphics/d3d12/descriptor.h"
#include "platform/windows/shared_ptr.h"

namespace mini::d3d12
{

struct SwapChainBuffer
{
    SharedPtr<ID3D12Resource> resource;
    Descriptor descriptor;
    D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc;

    SwapChainBuffer(D3D12_RENDER_TARGET_VIEW_DESC* = nullptr);
};

class SwapChain : public graphics::SwapChain
{
    static constexpr uint8 MaxBuffer = SwapChain::MaxBackBuffer;

private:
    SharedPtr<IDXGISwapChain3> m_SwapChain;
    StaticArray<SwapChainBuffer, MaxBuffer> m_Buffers;

    DXGI_SWAP_CHAIN_DESC1 m_SwapChainDesc;
    DXGI_SAMPLE_DESC m_SampleDesc;
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC m_FullscreenDesc;
    D3D12_RENDER_TARGET_VIEW_DESC m_RTVDesc;

    uint8 m_Index;
    uint8 m_VSync;
    uint32 m_Width;
    uint32 m_Height;

public:
    SwapChain();

    bool Initialize() final;
    void Present() final;

    void ResizeBackBuffer(uint32, uint32, bool) final;
    void SetBackBufferCount(uint8) final;
    void SetVSync(uint8) final;
    void SetFullScreen(bool) final;

    SwapChainBuffer* GetCurrentBuffer() noexcept { return &m_Buffers[m_Index]; }
    //Vector2Int GetBackBufferSize() const final { return Vector2Int(m_Width, m_Height); }
    uint8 GetBackBufferCount() const final { return (uint8)m_Buffers.Size(); }
    uint8 GetVSync() const final { return m_VSync; }
    bool GetFullscreen() const final { return !m_FullscreenDesc.Windowed; }

private:
    void ReleaseBuffers();
    void CreateBuffers(uint8);
};

} // namespace mini::d3d12
