export module mini.d3d12:device;

import mini.graphics;
import mini.windows;
import :swap_chain;
import :descriptor;

export namespace mini::d3d12 {

class Device final : public graphics::Device {
private:
    SharedPtr<IDXGIFactory4> m_factory;
    SharedPtr<IDXGIAdapter> m_adapter;
    SharedPtr<ID3D12Device> m_device;

    DescriptorAllocator m_rTVAllocator;
    DescriptorAllocator m_dSVAllocator;
    DescriptorAllocator m_sRVAllocator;

public:
    Device();

    bool Initialize() final;

    void CreateSwapChainBuffer(SwapChainBuffer&);
    graphics::SwapChain* CreateSwapChain() final;
    graphics::RenderContext* CreateRenderContext() final;

    graphics::API GetAPI() const final { return graphics::API::D3D12; }
    IDXGIFactory4* GetDXGIFactory() const { return m_factory; }
    IDXGIAdapter* GetDXGIAdapter() const { return m_adapter; }
    ID3D12Device* GetD3D12Device() const { return m_device; }

private:
    void CreateDevice(D3D_FEATURE_LEVEL = D3D_FEATURE_LEVEL_11_0);
    void EnableDebugLayer();
    void SetDebugLayerInfo();
};

} // namespace mini::d3d12