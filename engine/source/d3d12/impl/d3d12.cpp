module mini.d3d12;

import mini.graphics;
import mini.windows;

import :device;
import :renderer;
import :swap_chain;

namespace mini {

D3D12::D3D12() noexcept
    : m_windows("windows")
{
    d3d12::interface = this;
}

D3D12::~D3D12() noexcept
{
#if DEBUG
    SharedPtr<IDXGIDebug1> debug;
    ENSURE(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))) {
        debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
    }
#endif

    d3d12::interface = nullptr;
}

graphics::Device* D3D12::CreateDevice()
{
    return new d3d12::Device();
}

windows::Window* D3D12::GetWindow() noexcept
{
    return static_cast<windows::Window*>(m_windows->GetWindow());
}

d3d12::Device* D3D12::GetDevice() noexcept
{
    return static_cast<d3d12::Device*>(m_graphics->GetDevice());
}

d3d12::SwapChain* D3D12::GetSwapChain() noexcept
{
    return static_cast<d3d12::SwapChain*>(m_graphics->GetSwapChain());
}

d3d12::Renderer* D3D12::GetRenderer() noexcept
{
    return static_cast<d3d12::Renderer*>(m_graphics->GetRenderer());
}

} // namespace mini