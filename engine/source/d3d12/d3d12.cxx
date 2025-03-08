export module mini.d3d12;

export import :device;
export import :render_context;
export import :swap_chain;

extern "C" __declspec(dllexport) const char* D3D12SDKPath = "./";

extern "C" __declspec(dllexport) mini::graphics::Device* CreateGraphicDevice()
{
    return new mini::d3d12::Device();
}