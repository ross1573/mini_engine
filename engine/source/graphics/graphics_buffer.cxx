export module mini.graphics:buffer;

import mini.core;
import :resource;

namespace mini::graphics {

export enum class BufferCacheMode : byte {
    Default = 0,
    WriteCombined,
};

export enum class BufferStorageMode : byte {
    Private = 0,
    Shared,
};

export struct GRAPHICS_API BufferDescriptor {
public:
    size_t length = 0;
    BufferCacheMode cacheMode = BufferCacheMode::Default;
    BufferStorageMode storageMode = BufferStorageMode::Private;

public:
    constexpr BufferDescriptor() noexcept = default;

    explicit constexpr BufferDescriptor(size_t length) noexcept
        : length(length)
    {
    }

    constexpr BufferDescriptor(size_t length, BufferCacheMode cacheMode, BufferStorageMode storageMode) noexcept
        : length(length)
        , cacheMode(cacheMode)
        , storageMode(storageMode)
    {
    }
};

export class GRAPHICS_API Buffer : public Resource {
public:
    ~Buffer() noexcept override = default;
};

} // namespace mini::graphics