module mini.d3d12;

import mini.core;
import mini.windows;
import mini.engine;
import :log;
import :swap_chain;
import :command_queue;
import :renderer;

namespace mini::d3d12 {

Device::Device()
    : m_factory(nullptr)
    , m_adapter(nullptr)
    , m_device(nullptr)
    , m_RTVAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 256)
    , m_DSVAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1024)
    , m_SRVAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024)
{
}

bool Device::Initialize()
{
    ASSERT(m_factory == nullptr, "DXGIFactory not released");

    uint32 debugFlag = options::debugLayer ? DXGI_CREATE_FACTORY_DEBUG : 0;

    VERIFY(CreateDXGIFactory2(debugFlag, IID_PPV_ARGS(&m_factory)));

    if (debugFlag) {
        EnableDebugLayer();
    }

    CreateDevice();
    ENSURE(m_device, "D3D12 device not created") {
        return false;
    }

    if (debugFlag) {
        SetDebugLayerInfo();
    }

    ENSURE(m_RTVAllocator.Initialize(m_device), "RTV allocator init failed") return false;
    ENSURE(m_DSVAllocator.Initialize(m_device), "DSV allocator init failed") return false;
    ENSURE(m_SRVAllocator.Initialize(m_device), "SRV allocator init failed") return false;

    return true;
}

graphics::SwapChain* Device::CreateSwapChain()
{
    return new SwapChain();
}

graphics::Renderer* Device::CreateRenderer()
{
    return new Renderer(m_device.Get());
}

void Device::CreateSwapChainBuffer(SwapChainBuffer& buffer)
{
    buffer.descriptor = m_RTVAllocator.Allocate();
    m_device->CreateRenderTargetView(buffer.resource, buffer.rtvDesc, buffer.descriptor.offset);
}

void Device::CreateDevice(D3D_FEATURE_LEVEL minimum)
{
    ASSERT(m_device == nullptr);
    ASSERT(minimum >= D3D_FEATURE_LEVEL_11_0, "unsupported D3D12 feature level {}", static_cast<size_t>(minimum));

    D3D_FEATURE_LEVEL supportedFeatureLevel{ };
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_12_2,
                                          D3D_FEATURE_LEVEL_12_1,
                                          D3D_FEATURE_LEVEL_12_0,
                                          D3D_FEATURE_LEVEL_11_1,
                                          D3D_FEATURE_LEVEL_11_0 };

    int32 levelCount = sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL);
    int32 levelIdx = 0;

    for (; levelIdx < levelCount && m_device == nullptr; ++levelIdx) {
        supportedFeatureLevel = featureLevels[levelIdx];
        if (supportedFeatureLevel < minimum) {
            LogError("unable to find DirectX12 supported hardware");
            return;
        }

        DXGI_ADAPTER_DESC adapterDesc{ };
        SharedPtr<IDXGIAdapter> adapter = nullptr;
        SharedPtr<ID3D12Device> device = nullptr;

        for (uint32 idx = 0;; ++idx, adapter.Reset(), device.Reset()) {
            if (FAILED(m_factory->EnumAdapters(idx, &adapter))) {
                break;
            }

            if (FAILED(D3D12CreateDevice(adapter, supportedFeatureLevel, IID_PPV_ARGS(&device))) ||
                FAILED(adapter->GetDesc(&adapterDesc))) {
                continue;
            }

            if (adapterDesc.VendorId == 0x1414 ||
                lstrcmpW(adapterDesc.Description, L"Microsoft Basic Render Driver") == 0) {
                continue;
            }

            m_adapter = MoveArg(adapter);
            m_device = MoveArg(device);
            break;
        }
    }

    if (m_device == nullptr) {
        LogError("failed creating D3D12 device");
        return;
    }

    D3D_SHADER_MODEL supportedShaderModel = D3D_SHADER_MODEL_NONE;
    D3D12_FEATURE_DATA_SHADER_MODEL featureShaderModel{ };
    if (SUCCEEDED(m_device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL,
                                                &featureShaderModel,
                                                sizeof(featureShaderModel)))) {
        supportedShaderModel = featureShaderModel.HighestShaderModel;
    }

    StringView supportedFeatureLevelStr{ };
    switch (supportedFeatureLevel) {
        case D3D_FEATURE_LEVEL_12_2: supportedFeatureLevelStr = "D3D_FEATURE_LEVEL_12_2"; break;
        case D3D_FEATURE_LEVEL_12_1: supportedFeatureLevelStr = "D3D_FEATURE_LEVEL_12_1"; break;
        case D3D_FEATURE_LEVEL_12_0: supportedFeatureLevelStr = "D3D_FEATURE_LEVEL_12_0"; break;
        case D3D_FEATURE_LEVEL_11_1: supportedFeatureLevelStr = "D3D_FEATURE_LEVEL_11_1"; break;
        case D3D_FEATURE_LEVEL_11_0: supportedFeatureLevelStr = "D3D_FEATURE_LEVEL_11_0"; break;
        default:                     supportedFeatureLevelStr = "unsupported feature level"; break;
    }

    StringView supportedShaderModelStr{ };
    switch (supportedShaderModel) {
        case D3D_SHADER_MODEL_5_1:  supportedShaderModelStr = "D3D_SHADER_MODEL_5_1"; break;
        case D3D_SHADER_MODEL_6_0:  supportedShaderModelStr = "D3D_SHADER_MODEL_6_0"; break;
        case D3D_SHADER_MODEL_6_1:  supportedShaderModelStr = "D3D_SHADER_MODEL_6_1"; break;
        case D3D_SHADER_MODEL_6_2:  supportedShaderModelStr = "D3D_SHADER_MODEL_6_2"; break;
        case D3D_SHADER_MODEL_6_3:  supportedShaderModelStr = "D3D_SHADER_MODEL_6_3"; break;
        case D3D_SHADER_MODEL_6_4:  supportedShaderModelStr = "D3D_SHADER_MODEL_6_4"; break;
        case D3D_SHADER_MODEL_6_5:  supportedShaderModelStr = "D3D_SHADER_MODEL_6_5"; break;
        case D3D_SHADER_MODEL_6_6:  supportedShaderModelStr = "D3D_SHADER_MODEL_6_6"; break;
        case D3D_SHADER_MODEL_6_7:  supportedShaderModelStr = "D3D_SHADER_MODEL_6_7"; break;
        case D3D_SHADER_MODEL_6_8:  supportedShaderModelStr = "D3D_SHADER_MODEL_6_8"; break;
        case D3D_SHADER_MODEL_6_9:  supportedShaderModelStr = "D3D_SHADER_MODEL_6_9"; break;
        case D3D_SHADER_MODEL_NONE:
        default:                    supportedShaderModelStr = "unsupported shader model"; break;
    }

    DXGI_ADAPTER_DESC adapterDesc{ };
    m_adapter->GetDesc(&adapterDesc);

    const auto desc = StringConvert(adapterDesc.Description);
    const auto gpumem = adapterDesc.DedicatedVideoMemory;
    const auto sysmem = adapterDesc.DedicatedSystemMemory + adapterDesc.SharedSystemMemory;

    LogInfo("selected device: {} ({})", desc, adapterDesc.DeviceId);
    LogInfo("D3D feature level: {}", supportedFeatureLevelStr);
    LogInfo("D3D shader model: {}", supportedShaderModelStr);
    LogInfo("gpu memory: {} ({:4.2f} GB)", gpumem, (float32)gpumem / (float32)(1 << 30));
    LogInfo("system Memory: {} ({:4.2f} GB)", sysmem, (float32)sysmem / (float32)(1 << 30));
}

void Device::EnableDebugLayer()
{
    SharedPtr<ID3D12Debug> debugCtrl = nullptr;
    ENSURE(D3D12GetDebugInterface(IID_PPV_ARGS(&debugCtrl)), "failed to enable debug layer.") {
        return;
    }

    debugCtrl->EnableDebugLayer();

    if (mini::options::gpuValidation) {
        SharedPtr<ID3D12Debug5> debugCtrl5 = DynamicCast<ID3D12Debug5>(debugCtrl);
        ENSURE(debugCtrl5, "failed to enable GPU validation") {
            return;
        }

        debugCtrl5->SetEnableGPUBasedValidation(TRUE);
        debugCtrl5->SetEnableAutoName(TRUE);
    }
}

void Device::SetDebugLayerInfo()
{
    D3D12_INFO_QUEUE_FILTER filter = { };
    D3D12_MESSAGE_ID hide[] = { D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
                                D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
                                // Workarounds for debug layer issues on hybrid-graphics systems
                                D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
                                D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE };

    SharedPtr<ID3D12InfoQueue> infoQueue = DynamicCast<ID3D12InfoQueue>(m_device);
    ENSURE(infoQueue, "failed to set debug layer info.") {
        return;
    }

    infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
    infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);

    filter.DenyList.NumIDs = sizeof(hide) / sizeof(D3D12_MESSAGE_ID);
    filter.DenyList.pIDList = hide;
    infoQueue->AddStorageFilterEntries(&filter);
}

} // namespace mini::d3d12