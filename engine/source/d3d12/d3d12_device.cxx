export module mini.d3d12:device;

import mini.core;
import mini.graphics;
import mini.windows;
import :swap_chain;
import :descriptor;

export namespace mini::d3d12 {

class Device final : public graphics::Device {
private:
    SharedPtr<IDXGIFactory4> m_Factory;
    SharedPtr<IDXGIAdapter> m_Adapter;
    SharedPtr<ID3D12Device> m_Device;

    DescriptorAllocator m_RTVAllocator;
    DescriptorAllocator m_DSVAllocator;
    DescriptorAllocator m_SRVAllocator;

public:
    Device();

    bool Initialize() final;

    void CreateSwapChainBuffer(SwapChainBuffer&);
    graphics::SwapChain* CreateSwapChain() final;
    graphics::RenderContext* CreateRenderContext() final;

    inline graphics::API GetAPI() const final { return graphics::API::D3D12; }
    inline IDXGIFactory4* GetDXGIFactory() const { return m_Factory; }
    inline IDXGIAdapter* GetDXGIAdapter() const { return m_Adapter; }
    inline ID3D12Device* GetD3D12Device() const { return m_Device; }

private:
    void CreateDevice(D3D_FEATURE_LEVEL = D3D_FEATURE_LEVEL_11_0);
    void EnableDebugLayer();
    void SetDebugLayerInfo();
};

} // namespace mini::d3d12