export module mini.graphics:texture;

import mini.core;
import :common;
import :resource;

namespace mini::graphics {

export enum class TextureUsage : byte {
    undefined = 0,
    shaderRead = 1 << 0,
    shaderWrite = 1 << 1,
    renderTarget = 1 << 2,
};

export enum class TextureStorage : byte {
    gpuLocal,
    cpuWrite,
    cpuRead,
    transient,
};

export struct GRAPHICS_API TextureSwizzle {
public:
    enum class Channel : byte {
        zero = 0,
        one,
        red,
        green,
        blue,
        alpha,
    };

    Channel red = Channel::red;
    Channel green = Channel::green;
    Channel blue = Channel::blue;
    Channel alpha = Channel::alpha;
};

export struct GRAPHICS_API Texture2DDescriptor {
public:
    size_t width = 0;
    size_t height = 0;
    size_t mipmap = 1;

    PixelFormat pixelFormat = PixelFormat::RGBA8unorm;
    TextureUsage usage = TextureUsage::shaderRead;
};

export class GRAPHICS_API Texture : public Resource {
public:
    ~Texture() noexcept override = default;
};

export class GRAPHICS_API Texture2D : public Texture {
public:
    ~Texture2D() noexcept override = default;
};

} // namespace mini::graphics