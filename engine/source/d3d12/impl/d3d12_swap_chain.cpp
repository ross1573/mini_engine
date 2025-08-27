module mini.d3d12;

import mini.core;
import mini.platform;
import mini.windows;
import :device;
import :descriptor;
import :swap_chain;
import :command_queue;
import :render_context;

namespace mini::d3d12 {

SwapChainBuffer::SwapChainBuffer(D3D12_RENDER_TARGET_VIEW_DESC* rtv)
    : resource(nullptr)
    , descriptor{}
    , rtvDesc(rtv)
{
}

SwapChain::SwapChain()
    : m_SwapChain(nullptr)
    , m_Buffers()
    , m_SwapChainDesc{}
    , m_SampleDesc{}
    , m_FullscreenDesc{}
    , m_RTVDesc{}
    , m_Index(0)
    , m_VSync(0)
    , m_Width(0)
    , m_Height(0)
{
}

bool SwapChain::Initialize()
{
    uint8 bufferCount = options::bufferCount;
    bool fullscreen = options::fullscreen;
    m_VSync = options::vsync;

    SharedPtr<IDXGISwapChain1> swapChain;
    auto window = (windows::Window*)Platform::GetWindow();
    auto factory = ((Device*)Graphics::GetDevice())->GetDXGIFactory();
    auto renderContext = (RenderContext*)Graphics::GetRenderContext();
    auto commandQueue = renderContext->GetCommandQueue()->GetD3D12CommandQueue();

    RectInt size = window->GetSize();
    m_Width = static_cast<uint32>(size.width);
    m_Height = static_cast<uint32>(size.height);
    ASSERT((m_Width * m_Height) > 0, "invalid width and height");

    m_RTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    m_RTVDesc.Texture2D.MipSlice = 0;

    m_SampleDesc.Count = 1;
    m_SampleDesc.Quality = 0;
    m_FullscreenDesc.Windowed = !fullscreen;

    m_SwapChainDesc.Width = m_Width;
    m_SwapChainDesc.Height = m_Height;
    m_SwapChainDesc.Format = m_RTVDesc.Format;
    m_SwapChainDesc.SampleDesc = m_SampleDesc;
    m_SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    m_SwapChainDesc.BufferCount = (uint32)bufferCount;
    m_SwapChainDesc.Scaling = DXGI_SCALING_NONE;
    m_SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    m_SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    m_SwapChainDesc.Flags = m_VSync == 0 ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

    ENSURE(factory->CreateSwapChainForHwnd(commandQueue, window->GetHWND(), &m_SwapChainDesc,
                                           &m_FullscreenDesc, nullptr, &swapChain),
           "failed to create DXGI swapchain") {
        return false;
    }

    m_SwapChain = DynamicCast<IDXGISwapChain3>(swapChain);
    ENSURE(m_SwapChain, "IDXGISwapChain3 not supported") return false;

    CreateBuffers(bufferCount);
    return true;
}

void SwapChain::Present()
{
    m_SwapChain->Present(m_VSync, (m_VSync == 0) ? DXGI_PRESENT_ALLOW_TEARING : UINT(0));
    m_Index = (uint8)m_SwapChain->GetCurrentBackBufferIndex();
}

void SwapChain::ResizeBackBuffer(uint32 width, uint32 height, bool fullscreen)
{
    if (width * height <= 0) {
        log::Error("Cannot resize to {} x {}", width, height);
        return;
    }

    Graphics::GetRenderContext()->WaitForIdle();

    m_Width = width;
    m_Height = height;
    m_FullscreenDesc.Windowed = !fullscreen;
    uint8 bufferCount = (uint8)m_Buffers.Size();

    ReleaseBuffers();
    CreateBuffers(bufferCount);
}

void SwapChain::SetBackBufferCount(uint8 count)
{
    ENSURE(count > 0 || count < MaxBackBuffer, "Invalid swap chain count") return;

    ReleaseBuffers();
    CreateBuffers(count);
}

void SwapChain::SetVSync(uint8 vSync)
{
    m_VSync = vSync;
}

void SwapChain::SetFullScreen(bool fullscreen)
{
    Graphics::GetRenderContext()->WaitForIdle();

    if (FAILED(m_SwapChain->SetFullscreenState(fullscreen, nullptr))) {
        log::Error("Failed to set fullscreen state");
    }
    else {
        m_FullscreenDesc.Windowed = !fullscreen;
    }

    m_Index = (uint8)m_SwapChain->GetCurrentBackBufferIndex();
}

void SwapChain::ReleaseBuffers()
{
    m_Buffers.Clear();
    m_Index = 0;
}

void SwapChain::CreateBuffers(uint8 count)
{
    ASSERT(m_Buffers.Size() == 0, "swap chain buffer not released");
    ENSURE(m_Width * m_Height > 0, "invalid width and height") return;

    Device* device = (Device*)Graphics::GetDevice();
    uint32 swapChainFlags = (m_VSync == 0) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
    DXGI_FORMAT formatFlag = DXGI_FORMAT_R8G8B8A8_UNORM;

    m_SwapChain->SetFullscreenState(!m_FullscreenDesc.Windowed, nullptr);
    VERIFY(m_SwapChain->ResizeBuffers((uint)count, m_Width, m_Height, formatFlag, swapChainFlags),
           "failed to create swap chain buffers");

    for (uint32 i = 0; i < count; ++i) {
        SwapChainBuffer buffer(&m_RTVDesc);

        m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&buffer.resource));
        device->CreateSwapChainBuffer(buffer);
        m_Buffers.Push(MoveArg(buffer));
    }

    m_Index = (uint8)m_SwapChain->GetCurrentBackBufferIndex();
}

} // namespace mini::d3d12