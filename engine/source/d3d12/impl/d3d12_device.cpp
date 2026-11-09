module mini.d3d12;

import mini.engine;
import :log;
import :device;
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
    uint32 debugFlag = options::debugLayer ? DXGI_CREATE_FACTORY_DEBUG : 0;
    SharedPtr<DXGIFactory> factory;
    SharedPtr<DXGIAdapter> adapter;
    SharedPtr<D3D12Device> device;

    if (FAILED(CreateDXGIFactory2(debugFlag, COM_ARG(&factory)))) {
        VERIFY(CreateDXGIFactory2(0, COM_ARG(&factory)));
        debugFlag = 0;
    }

    if (debugFlag != 0u) {
        enableDebugLayer();
    }

    createDevice(D3D_FEATURE_LEVEL_11_0, factory, adapter, device);
    ENSURE(device, "D3D12 device not created") return;

    if (debugFlag != 0u) {
        setDebugLayerInfo(device);
    }

    ENSURE(m_RTVAllocator.Initialize(device), "RTV allocator init failed") return;
    ENSURE(m_DSVAllocator.Initialize(device), "DSV allocator init failed") return;
    ENSURE(m_SRVAllocator.Initialize(device), "SRV allocator init failed") return;

    m_factory = moveCast(factory);
    m_adapter = moveCast(adapter);
    m_device = moveCast(device);
}

graphics::SwapChain* Device::createSwapChain()
{
    return new SwapChain();
}

graphics::Renderer* Device::createRenderer()
{
    return new Renderer(this);
}

void Device::createSwapChainBuffer(SwapChainBuffer& buffer)
{
    buffer.descriptor = m_RTVAllocator.allocate();
    m_device->CreateRenderTargetView(buffer.resource, buffer.rtvDesc, buffer.descriptor.offset);
}

void Device::createDevice(D3D_FEATURE_LEVEL minFeatureLevel,
                          SharedPtr<DXGIFactory> const& factory,
                          SharedPtr<DXGIAdapter>& adapter,
                          SharedPtr<D3D12Device>& device)
{
    ASSERT(minFeatureLevel >= D3D_FEATURE_LEVEL_11_0,
           "unsupported D3D12 feature level {}",
           static_cast<size_t>(minFeatureLevel));

    D3D_FEATURE_LEVEL supportedFeatureLevel;
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    int32 levelCount = sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL);
    int32 levelIdx = 0;

    for (; levelIdx < levelCount && device == nullptr; ++levelIdx) {
        supportedFeatureLevel = featureLevels[levelIdx];
        if (supportedFeatureLevel < minFeatureLevel) {
            continue;
        }

        DXGI_ADAPTER_DESC adapterDesc{};
        adapter = nullptr;
        device = nullptr;

        for (uint32 idx = 0;; ++idx, adapter.reset(), device.reset()) {
            if (FAILED(factory->EnumAdapters(idx, &adapter))) {
                break;
            }

            if (FAILED(D3D12CreateDevice(adapter, supportedFeatureLevel, COM_ARG(&device))) ||
                FAILED(adapter->GetDesc(&adapterDesc))) {
                continue;
            }

            if (adapterDesc.VendorId == 0x1414 ||
                lstrcmpW(adapterDesc.Description, L"Microsoft Basic Render Driver") == 0) {
                continue;
            }

            break;
        }
    }

    if (!device.valid()) {
        logError("failed creating D3D12 device");
        return;
    }

    D3D12_FEATURE_DATA_SHADER_MODEL featureShaderModel{
        .HighestShaderModel = D3D_SHADER_MODEL_6_6,
    };
    VERIFY(device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &featureShaderModel, sizeof(featureShaderModel)));

    StringView supportedFeatureLevelStr{};
    switch (supportedFeatureLevel) {
        case D3D_FEATURE_LEVEL_12_2: supportedFeatureLevelStr = "D3D_FEATURE_LEVEL_12_2"; break;
        case D3D_FEATURE_LEVEL_12_1: supportedFeatureLevelStr = "D3D_FEATURE_LEVEL_12_1"; break;
        case D3D_FEATURE_LEVEL_12_0: supportedFeatureLevelStr = "D3D_FEATURE_LEVEL_12_0"; break;
        case D3D_FEATURE_LEVEL_11_1: supportedFeatureLevelStr = "D3D_FEATURE_LEVEL_11_1"; break;
        case D3D_FEATURE_LEVEL_11_0: supportedFeatureLevelStr = "D3D_FEATURE_LEVEL_11_0"; break;
        default:                     supportedFeatureLevelStr = "unsupported feature level"; break;
    }

    StringView supportedShaderModelStr{};
    switch (featureShaderModel.HighestShaderModel) {
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

    DXGI_ADAPTER_DESC adapterDesc{};
    adapter->GetDesc(&adapterDesc);

    const auto desc = convert<char>(adapterDesc.Description);
    const auto gpumem = adapterDesc.DedicatedVideoMemory;
    const auto sysmem = adapterDesc.DedicatedSystemMemory + adapterDesc.SharedSystemMemory;

    logInfo("selected device: {} ({})", desc, adapterDesc.DeviceId);
    logInfo("D3D feature level: {}", supportedFeatureLevelStr);
    logInfo("D3D shader model: {}", supportedShaderModelStr);
    logInfo("gpu memory: {} ({:4.2f} GB)", gpumem, (float32)gpumem / (float32)(1 << 30));
    logInfo("system Memory: {} ({:4.2f} GB)", sysmem, (float32)sysmem / (float32)(1 << 30));
}

void Device::enableDebugLayer()
{
    SharedPtr<ID3D12Debug> debugCtrl = nullptr;
    ENSURE(D3D12GetDebugInterface(COM_ARG(&debugCtrl)), "failed to enable debug layer.") {
        return;
    }

    debugCtrl->EnableDebugLayer();

    if (mini::options::gpuValidation) {
        SharedPtr<ID3D12Debug5> debugCtrl5 = dynamicCast<ID3D12Debug5>(debugCtrl);
        ENSURE(debugCtrl5, "failed to enable GPU validation") {
            return;
        }

        debugCtrl5->SetEnableGPUBasedValidation(TRUE);
        debugCtrl5->SetEnableAutoName(TRUE);
    }
}

void Device::setDebugLayerInfo(SharedPtr<D3D12Device>& device)
{
    D3D12_INFO_QUEUE_FILTER filter = {};
    D3D12_MESSAGE_ID hide[] = {
        D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
        D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
        // Workarounds for debug layer issues on hybrid-graphics systems
        D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
        D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
    };

    SharedPtr<ID3D12InfoQueue> infoQueue = dynamicCast<ID3D12InfoQueue>(device);
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