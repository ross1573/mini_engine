export module mini.metal;

export import :device;

extern "C" METAL_API mini::graphics::Device* CreateGraphicDevice()
{
    return new mini::metal::Device();
}