export module mini.d3d12:device;

import mini.graphics;
import mini.windows;
import :swap_chain;
import :descriptor;

D3D12_EXPORT(IDXGIAdapter);
D3D12_EXPORT_4(IDXGIFactory);
D3D12_EXPORT_4(ID3D12Device);
D3D12_EXPORT_5(ID3D12Debug);
D3D12_EXPORT(ID3D12InfoQueue);

export namespace mini::d3d12 {

class Device final : public graphics::Device {
public:
    typedef IDXGIFactory4 DXGIFactory;
    typedef IDXGIAdapter DXGIAdapter;
    typedef ID3D12Device4 D3D12Device;

private:
    SharedPtr<DXGIFactory> m_factory;
    SharedPtr<DXGIAdapter> m_adapter;
    SharedPtr<D3D12Device> m_device;

    DescriptorAllocator m_RTVAllocator;
    DescriptorAllocator m_DSVAllocator;
    DescriptorAllocator m_SRVAllocator;

public:
    Device();

    void createSwapChainBuffer(SwapChainBuffer&);
    [[nodiscard]] graphics::SwapChain* createSwapChain() final;
    [[nodiscard]] graphics::Renderer* createRenderer() final;

    [[nodiscard]] bool valid() const noexcept final { return m_device.valid(); }
    [[nodiscard]] graphics::Api api() const final { return graphics::Api::d3d12; }

    [[nodiscard]] DXGIFactory* dxgiFactory() const { return m_factory.get(); }
    [[nodiscard]] DXGIAdapter* dxgiAdapter() const { return m_adapter.get(); }
    [[nodiscard]] D3D12Device* d3d12Device() const { return m_device.get(); }

private:
    static void createDevice(D3D_FEATURE_LEVEL,
                             SharedPtr<DXGIFactory> const& factory,
                             SharedPtr<DXGIAdapter>& adapter,
                             SharedPtr<D3D12Device>& device);

    static void enableDebugLayer();
    static void setDebugLayerInfo(SharedPtr<D3D12Device>&);
};

} // namespace mini::d3d12