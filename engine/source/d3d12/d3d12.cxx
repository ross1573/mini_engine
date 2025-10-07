export module mini.d3d12;

export import :device;
export import :render_context;
export import :swap_chain;

extern "C" __declspec(dllexport) const char* D3D12SDKPath = "./";

namespace mini::d3d12 {

export class D3D12_API ModuleInterface : public mini::graphics::ModuleInterface {
public:
    mini::graphics::Device* CreateGraphicDevice() final { return new Device(); }
};

} // namespace mini::d3d12