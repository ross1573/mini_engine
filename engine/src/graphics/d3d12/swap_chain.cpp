#include "mini_engine.h"
#include "graphics/d3d12/device.h"
#include "graphics/d3d12/descriptor.h"
#include "graphics/d3d12/swap_chain.h"
#include "graphics/d3d12/command_queue.h"
#include "graphics/d3d12/render_context.h"
#include "platform/windows/window.h"
#include "platform/windows/utility.h"

namespace mini::d3d12
{

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
    auto window = Platform::GetWindow<windows::Window>();
    auto factory = Graphics::GetDevice<d3d12::Device>()->GetDXGIFactory();
    auto renderContext = Graphics::GetRenderContext<d3d12::RenderContext>();
    auto commandQueue = renderContext->GetCommandQueue();

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

    HRESULT hResult = E_FAIL;
    hResult = factory->CreateSwapChainForHwnd(commandQueue->GetD3D12CommandQueue(),
                                              window->GetHWND(),
                                              &m_SwapChainDesc,
                                              &m_FullscreenDesc,
                                              nullptr,
                                              &swapChain);

    ASSERT(hResult, "failed to create DXGI swapchain");

    m_SwapChain = DynamicCast<IDXGISwapChain3>(swapChain);
    ASSERT(m_SwapChain, "IDXGISwapChain3 not supported.");

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
    ENSURE (width * height > 0)
    {
        Log::Error("Cannot resize to {} x {}", width, height);
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
    ENSURE(count > 0 || count < MaxBackBuffer)
    {
        Log::Error("Invalid swap chain count {}", count);
        return;
    }

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

    ENSURE(m_SwapChain->SetFullscreenState(fullscreen, nullptr))
    {
        Log::Error("Failed to set fullscreen state");
    }
    else
    {
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
    [[maybe_unused]] HRESULT hResult;

    if (m_Width * m_Height <= 0)
    {
        return;
    }

    Device* device = Graphics::GetDevice<d3d12::Device>();
    uint32 swapChainFlags = (m_VSync == 0) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

    VERIFY(m_SwapChain->SetFullscreenState(!m_FullscreenDesc.Windowed, nullptr));
    VERIFY(m_SwapChain->ResizeBuffers((uint)count,
                                      m_Width,
                                      m_Height,
                                      DXGI_FORMAT_R8G8B8A8_UNORM,
                                      swapChainFlags), "failed to create swap chain buffers");

    for (uint32 i = 0; i < count; ++i)
    {
        SwapChainBuffer buffer(&m_RTVDesc);

        m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&buffer.resource));
        device->CreateSwapChainBuffer(buffer);
        m_Buffers.Push(MoveArg(buffer));
    }

    m_Index = (uint8)m_SwapChain->GetCurrentBackBufferIndex();
}

} // namespace mini::d3d12