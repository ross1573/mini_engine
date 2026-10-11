module mini.d3d12;

import mini.core;
import mini.platform;
import mini.windows;

import :log;
import :device;
import :descriptor;
import :swap_chain;
import :command_queue;
import :renderer;

namespace mini::d3d12 {

SwapChainBuffer::SwapChainBuffer(D3D12_RENDER_TARGET_VIEW_DESC* rtv)
    : resource(nullptr)
    , descriptor{}
    , rtvDesc(rtv)
{
}

SwapChain::SwapChain()
    : m_swapChain(nullptr)
    , m_buffers()
    , m_swapChainDesc{}
    , m_sampleDesc{}
    , m_fullscreenDesc{}
    , m_rTVDesc{}
    , m_index(0)
    , m_vSync(0)
    , m_width(0)
    , m_height(0)
{
    uint8 bufferCount = options::bufferCount;
    bool fullscreen = options::fullscreen;
    m_vSync = options::setVSync;

    SharedPtr<IDXGISwapChain1> swapChain;
    auto window = interface->window();
    auto factory = interface->device()->GetDXGIFactory();
    auto renderer = interface->renderer();
    auto commandQueue = renderer->GetCommandQueue()->GetD3D12CommandQueue();

    RectInt size = window->size();
    m_width = static_cast<uint32>(size.width);
    m_height = static_cast<uint32>(size.height);
    ASSERT((m_width * m_height) > 0, "invalid width and height");

    m_rTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_rTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    m_rTVDesc.Texture2D.MipSlice = 0;

    m_sampleDesc.Count = 1;
    m_sampleDesc.Quality = 0;
    m_fullscreenDesc.Windowed = !fullscreen;

    m_swapChainDesc.Width = m_width;
    m_swapChainDesc.Height = m_height;
    m_swapChainDesc.Format = m_rTVDesc.Format;
    m_swapChainDesc.SampleDesc = m_sampleDesc;
    m_swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    m_swapChainDesc.BufferCount = (uint32)bufferCount;
    m_swapChainDesc.Scaling = DXGI_SCALING_NONE;
    m_swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    m_swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    m_swapChainDesc.Flags = m_vSync == 0 ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

    ENSURE(factory->CreateSwapChainForHwnd(commandQueue,
                                           window->GetHWND(),
                                           &m_swapChainDesc,
                                           &m_fullscreenDesc,
                                           nullptr,
                                           &swapChain),
           "failed to create DXGI swapchain") {
        return;
    }

    m_swapChain = dynamicCast<IDXGISwapChain3>(swapChain);
    ENSURE(m_swapChain, "IDXGISwapChain3 not supported") return;

    CreateBuffers(bufferCount);
}

void SwapChain::present()
{
    m_swapChain->present(m_vSync, (m_vSync == 0) ? DXGI_PRESENT_ALLOW_TEARING : UINT(0));
    m_index = (uint8)m_swapChain->GetCurrentBackBufferIndex();
}

void SwapChain::resizeFrameBuffer(uint32 width, uint32 height, bool fullscreen)
{
    if (width * height <= 0) {
        logError("Cannot resize to {} x {}", width, height);
        return;
    }

    interface->renderer()->waitIdle();

    m_width = width;
    m_height = height;
    m_fullscreenDesc.Windowed = !fullscreen;
    uint8 bufferCount = (uint8)m_buffers.size();

    ReleaseBuffers();
    CreateBuffers(bufferCount);
}

void SwapChain::resizeBufferCount(uint8 count)
{
    ENSURE(count > 0 || count < MaxBackBuffer, "invalid swap chain count {}. max size is {}", count, MaxBackBuffer) {
        return;
    }

    ReleaseBuffers();
    CreateBuffers(count);
}

void SwapChain::setVSync(uint8 vSync)
{
    m_vSync = vSync;
}

void SwapChain::setFullScreen(bool fullscreen)
{
    interface->renderer()->waitIdle();

    if (FAILED(m_swapChain->SetFullscreenState(fullscreen, nullptr))) {
        logError("Failed to set fullscreen state");
    } else {
        m_fullscreenDesc.Windowed = !fullscreen;
    }

    m_index = (uint8)m_swapChain->GetCurrentBackBufferIndex();
}

void SwapChain::ReleaseBuffers()
{
    m_buffers.clear();
    m_index = 0;
}

void SwapChain::CreateBuffers(uint8 count)
{
    ASSERT(m_buffers.size() == 0, "swap chain buffer not released");
    ENSURE(m_width * m_height > 0, "invalid width and height") return;

    Device* device = interface->device();
    uint32 swapChainFlags = (m_vSync == 0) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
    DXGI_FORMAT formatFlag = DXGI_FORMAT_R8G8B8A8_UNORM;

    m_swapChain->SetFullscreenState(!m_fullscreenDesc.Windowed, nullptr);
    VERIFY(m_swapChain->ResizeBuffers((uint32)count, m_width, m_height, formatFlag, swapChainFlags),
           "failed to create swap chain buffers");

    for (uint32 i = 0; i < count; ++i) {
        SwapChainBuffer buffer(&m_rTVDesc);

        m_swapChain->GetBuffer(i, IID_PPV_ARGS(&buffer.resource));
        device->CreateSwapChainBuffer(buffer);
        m_buffers.pushBack(moveCast(buffer));
    }

    m_index = (uint8)m_swapChain->GetCurrentBackBufferIndex();
}

} // namespace mini::d3d12