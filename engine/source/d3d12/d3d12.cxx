export module mini.d3d12;

import mini.graphics;
import mini.windows;

export import :device;
export import :render_context;
export import :swap_chain;

extern "C" __declspec(dllexport) const char* D3D12SDKPath = "./";

namespace mini {

export class D3D12_API D3D12 final : public graphics::Interface {
private:
    Module<Windows> m_windows;

public:
    D3D12() noexcept;
    ~D3D12() noexcept;

    graphics::Device* CreateDevice() final;

    windows::Window* GetWindow() noexcept;
    d3d12::Device* GetDevice() noexcept;
    d3d12::SwapChain* GetSwapChain() noexcept;
    d3d12::RenderContext* GetRenderContext() noexcept;
};

} // namespace mini

namespace mini::d3d12 {

D3D12_API D3D12* interface = nullptr;

} // namespace mini::d3d12