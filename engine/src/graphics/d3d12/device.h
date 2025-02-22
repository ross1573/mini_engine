#pragma once

#include "core/utility.h"
#include "graphics/device.h"
#include "graphics/d3d12/descriptor.h"
#include "platform/windows/utility.h"

namespace mini::graphics
{
class SwapChain;
class RenderContext;
}

namespace mini::d3d12
{

struct SwapChainBuffer;

class Device : public graphics::Device
{
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

    graphics::SwapChain* CreateSwapChain() final;
    graphics::RenderContext* CreateRenderContext() final;

    void CreateSwapChainBuffer(SwapChainBuffer&);

    graphics::API GetAPI() const final { return graphics::API::D3D12; }
    IDXGIFactory4* GetDXGIFactory() const { return m_Factory.Get(); }
    IDXGIAdapter* GetDXGIAdapter() const { return m_Adapter.Get(); }
    ID3D12Device* GetD3D12Device() const { return m_Device.Get(); }

private:
    void CreateDevice(D3D_FEATURE_LEVEL = D3D_FEATURE_LEVEL_11_0);
    void EnableDebugLayer();
    void SetDebugLayerInfo();
};

} // namespace mini::d3d12