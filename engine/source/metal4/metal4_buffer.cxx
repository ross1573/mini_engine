module;

#include <Metal/MTLBuffer.hpp>

export module mini.metal4:buffer;

import mini.core;
import mini.graphics;
import :resource;
import :device;

export namespace MTL {

using MTL::Buffer;

} // namespace MTL

namespace mini::metal4 {

export METAL4_API constexpr MTL::ResourceOptions MTLBufferResourceOptions(
    graphics::BufferCacheMode cacheMode,
    graphics::BufferStorageMode storageMode) noexcept
{
    MTL::ResourceOptions options{0};

    switch (cacheMode) {
        case graphics::BufferCacheMode::WriteCombined: options |= MTL::ResourceCPUCacheModeWriteCombined; break;
        case graphics::BufferCacheMode::Default:       break;
        default:
            ASSERT(cacheMode == graphics::BufferCacheMode::Default,
                   "invalid resource cache mode {}.",
                   static_cast<byte>(cacheMode));
            break;
    }

    switch (storageMode) {
        case graphics::BufferStorageMode::Shared:
            if constexpr (ARCH_ARM64) {
                options |= MTL::ResourceStorageModeShared;
            } else {
                options |= MTL::ResourceStorageModeManaged;
            }
        case graphics::BufferStorageMode::Private: options |= MTL::ResourceStorageModePrivate; break;
        default:
            ASSERT(storageMode == graphics::BufferStorageMode::Shared,
                   "invalid resource storage mode {}.",
                   static_cast<byte>(storageMode));
            break;
    }

    options |= MTL::ResourceHazardTrackingModeUntracked;
    return options;
}

export class METAL4_API Buffer
    : public graphics::Buffer
    , public Resource {
public:
    explicit Buffer(MTL::Buffer* buffer);
    Buffer(Device* device, graphics::BufferDescriptor const& descriptor);
    ~Buffer() noexcept override = default;

    [[nodiscard]] size_t Size() const { return static_cast<size_t>(MTLBuffer()->length()); }
    [[nodiscard]] uint64 GpuAddress() const override { return static_cast<uint64>(MTLBuffer()->gpuAddress()); }

    [[nodiscard]] MTL::Buffer* MTLBuffer() const noexcept { return static_cast<MTL::Buffer*>(m_resource.Get()); }
};

} // namespace mini::metal4
