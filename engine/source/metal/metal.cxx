export module mini.metal;

export import :device;
export import :swap_chain;
export import :render_context;

extern "C" METAL_API mini::graphics::Device* CreateGraphicDevice()
{
    return new mini::metal::Device();
}