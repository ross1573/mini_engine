#include "mini_engine.h"
#include "graphics/d3d12/device.h"
#include "graphics/d3d12/swap_chain.h"
#include "graphics/d3d12/command_queue.h"
#include "graphics/d3d12/render_context.h"
#include "platform/windows/utility.h"

namespace mini::d3d12
{

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

    HRESULT hResult;
    uint32 debugFlag = options::debugLayer ? DXGI_CREATE_FACTORY_DEBUG : 0;

    hResult = CreateDXGIFactory2(debugFlag, IID_PPV_ARGS(&m_Factory));
    ASSERT(hResult, "failed to create DXGI factory");

    if (debugFlag)
    {
        EnableDebugLayer();
    }

    CreateDevice();
    ASSERT(m_Device, "D3D12 device not created");

    if (debugFlag)
    {
        SetDebugLayerInfo();
    }

    m_RTVAllocator.Initialize(m_Device);
    m_DSVAllocator.Initialize(m_Device);
    m_SRVAllocator.Initialize(m_Device);

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
    m_Device->CreateRenderTargetView(buffer.resource.Get(),
                                     buffer.rtvDesc,
                                     buffer.descriptor.offset);
}

void Device::CreateDevice(D3D_FEATURE_LEVEL minimum)
{
    ASSERT(m_Device == nullptr);
    ASSERT(minimum >= D3D_FEATURE_LEVEL_11_0, "unsupported D3D12 feature level");

    HRESULT hResult = E_FAIL;
    D3D_FEATURE_LEVEL selectedLevel{};
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,

        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };

    int levelCount = sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL);
    int levelIdx = 0;

    for (; levelIdx < levelCount && m_Device == nullptr; ++levelIdx)
    {
        selectedLevel = featureLevels[levelIdx];
        if (selectedLevel < minimum)
        {
            Log::Error("unable to find DirectX12 supported hardware");
            return;
        }

        SharedPtr<IDXGIAdapter> adapter = nullptr;
        SharedPtr<ID3D12Device> device = nullptr;

        for (uint idx = 0;
             SUCCEEDED(m_Factory->EnumAdapters(idx, &adapter));
             ++idx, adapter.Reset(), device.Reset())
        {
            hResult = D3D12CreateDevice(adapter, selectedLevel, IID_PPV_ARGS(&device));

            if (FAILED(hResult))
            {
                continue;
            }

            DXGI_ADAPTER_DESC adapterDesc{};
            if (FAILED(adapter->GetDesc(&adapterDesc)))
            {
                continue;
            }

            if (adapterDesc.VendorId == 0x1414 ||
                lstrcmpW(adapterDesc.Description, L"Microsoft Basic Render Driver") == 0)
            {
                continue;
            }

            m_Adapter = MoveArg(adapter);
            m_Device = MoveArg(device);
            break;
        }
    }

    if (FAILED(hResult) || m_Device == nullptr)
    {
        Log::Error("failed creating D3D12 device");
        return;
    }

    String selectedLevelStr{};
    switch (selectedLevel)
    {
        case D3D_FEATURE_LEVEL_12_2: selectedLevelStr = "D3D_FEATURE_LEVEL_12_2"; break;
        case D3D_FEATURE_LEVEL_12_1: selectedLevelStr = "D3D_FEATURE_LEVEL_12_1"; break;
        case D3D_FEATURE_LEVEL_12_0: selectedLevelStr = "D3D_FEATURE_LEVEL_12_0"; break;
        case D3D_FEATURE_LEVEL_11_1: selectedLevelStr = "D3D_FEATURE_LEVEL_11_1"; break;
        case D3D_FEATURE_LEVEL_11_0: selectedLevelStr = "D3D_FEATURE_LEVEL_11_0"; break;
        default: ASSERT(false);
    }

    DXGI_ADAPTER_DESC adapterDesc{};
    m_Adapter->GetDesc(&adapterDesc);

    const auto desc = windows::StringConverter<wchar_t, char>(adapterDesc.Description);
    const auto gpumem = adapterDesc.DedicatedVideoMemory;
    const auto sysmem = adapterDesc.DedicatedSystemMemory + adapterDesc.SharedSystemMemory;

    Log::Info("Selected device: {} ({})", desc.Data(), adapterDesc.DeviceId);
    Log::Info("D3D feature level: {}", selectedLevelStr);
    Log::Info("GPU Memory: {} ({:4.2f} GB)", gpumem, (float)gpumem / (float)(1 << 30));
    Log::Info("System Memory: {} ({:4.2f} GB)", sysmem, (float)sysmem / (float)(1 << 30));
}

void Device::EnableDebugLayer()
{
    SharedPtr<ID3D12Debug> debugController = nullptr;

    if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        Log::Error("failed to enable debug layer.");
        return;
    }

    debugController->EnableDebugLayer();

    if (mini::options::gpuValidation)
    {
        SharedPtr<ID3D12Debug5> debugController5 = nullptr;

        if (FAILED(debugController->QueryInterface(IID_PPV_ARGS(&debugController5))))
        {
            Log::Error("failed to enable GPU validation");
            return;
        }

        debugController5->SetEnableGPUBasedValidation(TRUE);
        debugController5->SetEnableAutoName(TRUE);
    }

#ifdef DEBUG
    Graphics::DebugFunc = []()
    {
        SharedPtr<IDXGIDebug1> debug;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
        {
            debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
        }
    };
#endif
}

void Device::SetDebugLayerInfo()
{
    SharedPtr<ID3D12InfoQueue> pInfoQueue = nullptr;

    D3D12_INFO_QUEUE_FILTER filter = {};
    D3D12_MESSAGE_ID hide[] =
    {
        D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
        D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,

        // Workarounds for debug layer issues on hybrid-graphics systems
        D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
        D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
    };

    if (FAILED(m_Device->QueryInterface(IID_PPV_ARGS(&pInfoQueue))))
    {
        Log::Error("failed to set debug layer info.");
        return;
    }

    pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
    pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);

    filter.DenyList.NumIDs = sizeof(hide) / sizeof(D3D12_MESSAGE_ID);
    filter.DenyList.pIDList = hide;
    pInfoQueue->AddStorageFilterEntries(&filter);
}

} // namespace mini::d3d12