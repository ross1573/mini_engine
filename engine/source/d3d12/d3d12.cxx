export module mini.d3d12;

export import :device;
export import :render_context;
export import :swap_chain;

extern "C" __declspec(dllexport) const char* D3D12SDKPath = "./";

namespace mini::d3d12 {

export class D3D12_API Interface final : public Graphics {
public:
    ~Interface() noexcept = default;

    graphics::Device* CreateDevice() final { return new Device(); }
};

} // namespace mini::d3d12