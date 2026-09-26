export module mini.graphics:texture;

import mini.core;
import :common;
import :resource;

namespace mini::graphics {

export enum class TextureType : byte {
    Texture1D = 0,
    Texture1DArray,
    Texture2D,
    Textrue2DArray,
    Texture3D,
    TextureCube,
    TextureCubeArray,
};

export enum class TextureUsage : byte {
    Undefined = 0,
    ShaderRead = 1 << 0,
    ShaderWrite = 1 << 1,
    RenderTarget = 1 << 2,
};

export enum class TextureCacheMode : byte {
    Default = 0,
    WriteCombined,
};

export enum class TextureStorageMode : byte {
    Private = 0,
    Shared,
    Transient,
};

export struct GRAPHICS_API TextureSwizzle {
public:
    enum class Channel : byte {
        Zero = 0,
        One,
        Red,
        Green,
        Blue,
        Alpha,
    };

    Channel red;
    Channel green;
    Channel blue;
    Channel alpha;

public:
    constexpr TextureSwizzle() noexcept
        : red(Channel::Red)
        , green(Channel::Green)
        , blue(Channel::Blue)
        , alpha(Channel::Alpha)
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
    TextureType type = TextureType::Texture2D;
    TextureUsage usage = TextureUsage::ShaderRead;
    TextureCacheMode cacheMode = TextureCacheMode::Default;
    TextureStorageMode storageMode = TextureStorageMode::Private;
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