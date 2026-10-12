export module mini.graphics:texture;

import mini.core;
import :common;
import :resource;

namespace mini::graphics {

export enum class TextureType : byte {
    texture1d = 0,
    texture1dArray,
    texture2d,
    textrue2dArray,
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

export enum class TextureCacheMode : byte {
    defaultCache = 0,
    writeCombined,
};

export enum class TextureStorageMode : byte {
    privateStorage = 0,
    sharedStorage,
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

    Channel red;
    Channel green;
    Channel blue;
    Channel alpha;

public:
    constexpr TextureSwizzle() noexcept
        : red(Channel::red)
        , green(Channel::green)
        , blue(Channel::blue)
        , alpha(Channel::alpha)
    {
    }

    constexpr TextureSwizzle(Channel r, Channel g, Channel b, Channel a) noexcept
        : red(r)
        , green(g)
        , blue(b)
        , alpha(a)
    {
    }
};

export struct GRAPHICS_API TextureDescriptor {
public:
    size_t width = 0;
    size_t height = 0;
    size_t depth = 1;
    size_t mipmapLevel = 1;

    PixelFormat pixelFormat = PixelFormat::RGBA8unorm;
    TextureType type = TextureType::texture2d;
    TextureUsage usage = TextureUsage::shaderRead;
    TextureCacheMode cacheMode = TextureCacheMode::defaultCache;
    TextureStorageMode storageMode = TextureStorageMode::privateStorage;
    TextureSwizzle swizzle;

public:
    constexpr TextureDescriptor() noexcept = default;

    constexpr TextureDescriptor(size_t width, size_t height, PixelFormat pixelFormat) noexcept
        : width(width)
        , height(height)
        , pixelFormat(pixelFormat)
    {
    }

    constexpr TextureDescriptor(size_t width,
                                size_t height,
                                size_t depth,
                                size_t mipmapLevel,
                                PixelFormat pixelFormat,
                                TextureType type,
                                TextureUsage usage,
                                TextureCacheMode cacheMode,
                                TextureStorageMode storageMode,
                                TextureSwizzle swizzle) noexcept
        : width(width)
        , height(height)
        , depth(depth)
        , mipmapLevel(mipmapLevel)
        , pixelFormat(pixelFormat)
        , type(type)
        , usage(usage)
        , cacheMode(cacheMode)
        , storageMode(storageMode)
        , swizzle(swizzle)
    {
    }
};

export class GRAPHICS_API Texture : public Resource {
public:
    ~Texture() noexcept override = default;
};

} // namespace mini::graphics