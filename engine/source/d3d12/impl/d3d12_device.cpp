module;

#include "assertion.h"
#include "option.h"

module mini.d3d12;

import mini.core;
import mini.windows;
import mini.engine;
import :swap_chain;
import :command_queue;
import :render_context;

namespace mini::d3d12 {

Device::Device()
    : m_Factory(nullptr)
    , m_Adapter(nullptr)
    , m_Device(nullptr)
    , m_RTVAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 256)
    , m_DSVAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1024)
    , m_SRVAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024)
{
}

bool Device::Initialize()
{
    ASSERT(m_Factory == nullptr, "DXGIFactory not released");

    uint32 debugFlag = options::debugLayer ? DXGI_CREATE_FACTORY_DEBUG : 0;

    VERIFY(CreateDXGIFactory2(debugFlag, IID_PPV_ARGS(&m_Factory)));

    if (debugFlag) {
        EnableDebugLayer();
    }

    CreateDevice();
    ENSURE(m_Device, "D3D12 device not created")
    {
        return false;
    }

    if (debugFlag) {
        SetDebugLayerInfo();
    }

    ENSURE(m_RTVAllocator.Initialize(m_Device)) return false;
    ENSURE(m_DSVAllocator.Initialize(m_Device)) return false;
    ENSURE(m_SRVAllocator.Initialize(m_Device)) return false;

    return true;
}

graphics::SwapChain* Device::CreateSwapChain()
{
    return new SwapChain();
}

graphics::RenderContext* Device::CreateRenderContext()
{
    return new RenderContext();
}

void Device::CreateSwapChainBuffer(SwapChainBuffer& buffer)
{
    buffer.descriptor = m_RTVAllocator.Allocate();
    m_Device->CreateRenderTargetView(buffer.resource, buffer.rtvDesc, buffer.descriptor.offset);
}

void Device::CreateDevice(D3D_FEATURE_LEVEL minimum)
{
    ASSERT(m_Device == nullptr);
    ASSERT(minimum >= D3D_FEATURE_LEVEL_11_0, "unsupported D3D12 feature level");

    D3D_FEATURE_LEVEL selectedLevel{};
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1,
                                          D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1,
                                          D3D_FEATURE_LEVEL_11_0 };

    int levelCount = sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL);
    int levelIdx = 0;

    for (; levelIdx < levelCount && m_Device == nullptr; ++levelIdx) {
        selectedLevel = featureLevels[levelIdx];
        if (selectedLevel < minimum) {
            log::Error("unable to find DirectX12 supported hardware");
            return;
        }

        DXGI_ADAPTER_DESC adapterDesc{};
        SharedPtr<IDXGIAdapter> adapter = nullptr;
        SharedPtr<ID3D12Device> device = nullptr;

        for (uint idx = 0;; ++idx, adapter.Reset(), device.Reset()) {
            if (FAILED(m_Factory->EnumAdapters(idx, &adapter))) {
                break;
            }

            if (FAILED(D3D12CreateDevice(adapter, selectedLevel, IID_PPV_ARGS(&device))) ||
                FAILED(adapter->GetDesc(&adapterDesc))) {
                continue;
            }

            if (adapterDesc.VendorId == 0x1414 ||
                lstrcmpW(adapterDesc.Description, L"Microsoft Basic Render Driver") == 0) {
                continue;
            }

            m_Adapter = MoveArg(adapter);
            m_Device = MoveArg(device);
            break;
        }
    }

    if (m_Device == nullptr) {
        log::Error("failed creating D3D12 device");
        return;
    }

    String selectedLevelStr{};
    switch (selectedLevel) {
        case D3D_FEATURE_LEVEL_12_2: selectedLevelStr = "D3D_FEATURE_LEVEL_12_2"; break;
        case D3D_FEATURE_LEVEL_12_1: selectedLevelStr = "D3D_FEATURE_LEVEL_12_1"; break;
        case D3D_FEATURE_LEVEL_12_0: selectedLevelStr = "D3D_FEATURE_LEVEL_12_0"; break;
        case D3D_FEATURE_LEVEL_11_1: selectedLevelStr = "D3D_FEATURE_LEVEL_11_1"; break;
        case D3D_FEATURE_LEVEL_11_0: selectedLevelStr = "D3D_FEATURE_LEVEL_11_0"; break;
        default:                     selectedLevelStr = "unsupported feature level"; break;
    }

    DXGI_ADAPTER_DESC adapterDesc{};
    m_Adapter->GetDesc(&adapterDesc);

    const auto desc = StringConvert<char>(adapterDesc.Description);
    const auto gpumem = adapterDesc.DedicatedVideoMemory;
    const auto sysmem = adapterDesc.DedicatedSystemMemory + adapterDesc.SharedSystemMemory;

    log::Info("selected device: {} ({})", desc.Data(), adapterDesc.DeviceId);
    log::Info("D3D feature level: {}", selectedLevelStr);
    log::Info("gpu memory: {} ({:4.2f} GB)", gpumem, (float)gpumem / (float)(1 << 30));
    log::Info("system Memory: {} ({:4.2f} GB)", sysmem, (float)sysmem / (float)(1 << 30));
}

void Device::EnableDebugLayer()
{
#if DEBUG
    Engine::AtExit([]() noexcept {
        SharedPtr<IDXGIDebug1> debug;
        ENSURE(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))) return;
        debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
    });
#endif

    SharedPtr<ID3D12Debug> debugCtrl = nullptr;
    ENSURE(D3D12GetDebugInterface(IID_PPV_ARGS(&debugCtrl)), "failed to enable debug layer.")
    {
        return;
    }

    debugCtrl->EnableDebugLayer();

    if (mini::options::gpuValidation) {
        SharedPtr<ID3D12Debug5> debugCtrl5 = DynamicCast<ID3D12Debug5>(debugCtrl);
        ENSURE(debugCtrl5, "failed to enable GPU validation")
        {
            return;
        }

        debugCtrl5->SetEnableGPUBasedValidation(TRUE);
        debugCtrl5->SetEnableAutoName(TRUE);
    }
}

void Device::SetDebugLayerInfo()
{
    D3D12_INFO_QUEUE_FILTER filter = {};
    D3D12_MESSAGE_ID hide[] = { D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
                                D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
                                // Workarounds for debug layer issues on hybrid-graphics systems
                                D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
                                D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE };

    SharedPtr<ID3D12InfoQueue> infoQueue = DynamicCast<ID3D12InfoQueue>(m_Device);
    ENSURE(infoQueue, "failed to set debug layer info.")
    {
        return;
    }

    infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
    infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);

    filter.DenyList.NumIDs = sizeof(hide) / sizeof(D3D12_MESSAGE_ID);
    filter.DenyList.pIDList = hide;
    infoQueue->AddStorageFilterEntries(&filter);
}

} // namespace mini::d3d12