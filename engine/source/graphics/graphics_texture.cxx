export module mini.graphics:texture;

import mini.core;
import :common;
import :resource;

namespace mini::graphics {

export enum class TextureType : byte {
    texture1d = 0,
    texture1dArray,
    texture2d,
    texture2dArray,
    texture3d,
    textureCube,
    textureCubeArray,
};

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

export struct GRAPHICS_API TextureDescriptor {
public:
    uint64 width = 0;
    uint64 height = 0;
    uint16 depth = 1;
    uint16 mipmapLevel = 1;

    PixelFormat pixelFormat = PixelFormat::RGBA8unorm;
    TextureType type = TextureType::texture2d;
    TextureUsage usage = TextureUsage::shaderRead;

public:
    constexpr TextureDescriptor() noexcept = default;
    constexpr TextureDescriptor(uint64 width,
                                uint64 height,
                                uint16 depth,
                                uint16 mipmapLevel,
                                PixelFormat pixelFormat,
                                TextureType type,
                                TextureUsage usage) noexcept
        : width(width)
        , height(height)
        , depth(depth)
        , mipmapLevel(mipmapLevel)
        , pixelFormat(pixelFormat)
        , type(type)
        , usage(usage)
    {
    }
};

export GRAPHICS_API constexpr TextureDescriptor texture1dDescriptor(uint64 width, PixelFormat pixelFormat) noexcept
{
    TextureDescriptor desc{};
    desc.width = width;
    desc.pixelFormat = pixelFormat;
    desc.type = TextureType::texture1d;
    return desc;
}

export GRAPHICS_API constexpr TextureDescriptor texture1dDescriptor(uint64 width,
                                                                    uint16 mipmapLevel,
                                                                    PixelFormat pixelFormat,
                                                                    TextureUsage usage) noexcept
{
    TextureDescriptor desc{};
    desc.width = width;
    desc.mipmapLevel = mipmapLevel;
    desc.pixelFormat = pixelFormat;
    desc.type = TextureType::texture1d;
    desc.usage = usage;
    return desc;
}

export GRAPHICS_API constexpr TextureDescriptor texture1dArrayDesciptor(uint64 width,
                                                                        uint16 arrayLength,
                                                                        PixelFormat pixelFormat) noexcept
{
    TextureDescriptor desc{};
    desc.width = width;
    desc.depth = arrayLength;
    desc.pixelFormat = pixelFormat;
    desc.type = TextureType::texture1dArray;
    return desc;
}

export GRAPHICS_API constexpr TextureDescriptor texture1dArrayDesciptor(uint64 width,
                                                                        uint16 arrayLength,
                                                                        uint16 mipmapLevel,
                                                                        PixelFormat pixelFormat,
                                                                        TextureUsage usage) noexcept
{
    TextureDescriptor desc{};
    desc.width = width;
    desc.depth = arrayLength;
    desc.mipmapLevel = mipmapLevel;
    desc.pixelFormat = pixelFormat;
    desc.type = TextureType::texture1dArray;
    desc.usage = usage;
    return desc;
}

export GRAPHICS_API constexpr TextureDescriptor texture2dDescriptor(uint64 width,
                                                                    uint64 height,
                                                                    PixelFormat pixelFormat) noexcept
{
    TextureDescriptor desc{};
    desc.width = width;
    desc.height = height;
    desc.pixelFormat = pixelFormat;
    desc.type = TextureType::texture2d;
    return desc;
}

export GRAPHICS_API constexpr TextureDescriptor texture2dDescriptor(uint64 width,
                                                                    uint64 height,
                                                                    uint16 mipmapLevel,
                                                                    PixelFormat pixelFormat,
                                                                    TextureUsage usage) noexcept
{
    TextureDescriptor desc{};
    desc.width = width;
    desc.height = height;
    desc.mipmapLevel = mipmapLevel;
    desc.pixelFormat = pixelFormat;
    desc.type = TextureType::texture2d;
    desc.usage = usage;
    return desc;
}

export GRAPHICS_API constexpr TextureDescriptor texture2dArrayDescriptor(uint64 width,
                                                                         uint64 height,
                                                                         uint16 arrayLength,
                                                                         PixelFormat pixelFormat) noexcept
{
    TextureDescriptor desc{};
    desc.width = width;
    desc.height = height;
    desc.depth = arrayLength;
    desc.pixelFormat = pixelFormat;
    desc.type = TextureType::texture2dArray;
    return desc;
}

export GRAPHICS_API constexpr TextureDescriptor texture2dArrayDescriptor(uint64 width,
                                                                         uint64 height,
                                                                         uint16 arrayLength,
                                                                         uint16 mipmapLevel,
                                                                         PixelFormat pixelFormat,
                                                                         TextureUsage usage) noexcept
{
    TextureDescriptor desc{};
    desc.width = width;
    desc.height = height;
    desc.depth = arrayLength;
    desc.mipmapLevel = mipmapLevel;
    desc.pixelFormat = pixelFormat;
    desc.type = TextureType::texture2dArray;
    desc.usage = usage;
    return desc;
}

export GRAPHICS_API constexpr TextureDescriptor texture3dDescriptor(uint64 width,
                                                                    uint64 height,
                                                                    uint16 depth,
                                                                    PixelFormat pixelFormat) noexcept
{
    TextureDescriptor desc{};
    desc.width = width;
    desc.height = height;
    desc.depth = depth;
    desc.pixelFormat = pixelFormat;
    desc.type = TextureType::texture3d;
    return desc;
}

export GRAPHICS_API constexpr TextureDescriptor texture3dDescriptor(uint64 width,
                                                                    uint64 height,
                                                                    uint16 depth,
                                                                    uint16 mipmapLevel,
                                                                    PixelFormat pixelFormat,
                                                                    TextureUsage usage) noexcept
{
    TextureDescriptor desc{};
    desc.width = width;
    desc.height = height;
    desc.depth = depth;
    desc.mipmapLevel = mipmapLevel;
    desc.pixelFormat = pixelFormat;
    desc.type = TextureType::texture3d;
    desc.usage = usage;
    return desc;
}

export GRAPHICS_API constexpr TextureDescriptor textureCubeDescriptor(uint64 width,
                                                                      uint64 height,
                                                                      PixelFormat pixelFormat) noexcept
{
    TextureDescriptor desc{};
    desc.width = width;
    desc.height = height;
    desc.pixelFormat = pixelFormat;
    desc.type = TextureType::textureCube;
    return desc;
}

export GRAPHICS_API constexpr TextureDescriptor textureCubeDescriptor(uint64 width,
                                                                      uint64 height,
                                                                      uint16 mipmapLevel,
                                                                      PixelFormat pixelFormat,
                                                                      TextureUsage usage) noexcept
{
    TextureDescriptor desc{};
    desc.width = width;
    desc.height = height;
    desc.mipmapLevel = mipmapLevel;
    desc.pixelFormat = pixelFormat;
    desc.type = TextureType::textureCube;
    desc.usage = usage;
    return desc;
}

export GRAPHICS_API constexpr TextureDescriptor textureCubeArrauDescriptor(uint64 width,
                                                                           uint64 height,
                                                                           uint16 arrayLength,
                                                                           PixelFormat pixelFormat) noexcept
{
    TextureDescriptor desc{};
    desc.width = width;
    desc.height = height;
    desc.depth = arrayLength;
    desc.pixelFormat = pixelFormat;
    desc.type = TextureType::textureCubeArray;
    return desc;
}

export GRAPHICS_API constexpr TextureDescriptor textureCubeDescriptor(uint64 width,
                                                                      uint64 height,
                                                                      uint16 arrayLength,
                                                                      uint16 mipmapLevel,
                                                                      PixelFormat pixelFormat,
                                                                      TextureUsage usage) noexcept
{
    TextureDescriptor desc{};
    desc.width = width;
    desc.height = height;
    desc.depth = arrayLength;
    desc.mipmapLevel = mipmapLevel;
    desc.pixelFormat = pixelFormat;
    desc.type = TextureType::textureCubeArray;
    desc.usage = usage;
    return desc;
}

export class GRAPHICS_API Texture : public Resource {
public:
    ~Texture() noexcept override = default;
};

} // namespace mini::graphics