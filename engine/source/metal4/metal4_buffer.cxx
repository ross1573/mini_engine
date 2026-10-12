module;

#include <Metal/MTLBuffer.hpp>

export module mini.metal4:buffer;

import mini.core;
import mini.graphics;
import :resource;

export namespace MTL {

using MTL::Buffer;

} // namespace MTL

namespace mini::metal4 {

class Device;

} // namespace mini::metal4

namespace mini::metal4 {

export METAL4_API constexpr MTL::ResourceOptions mtlBufferResourceOptions(
    graphics::BufferCacheMode cacheMode,
    graphics::BufferStorageMode storageMode) noexcept
{
    MTL::ResourceOptions options{0};

    switch (cacheMode) {
        case graphics::BufferCacheMode::writeCombined: options |= MTL::ResourceCPUCacheModeWriteCombined; break;
        case graphics::BufferCacheMode::defaultCache:  break;
        default:
            ASSERT(cacheMode == graphics::BufferCacheMode::defaultCache,
                   "invalid resource cache mode {}.",
                   static_cast<byte>(cacheMode));
            break;
    }

    switch (storageMode) {
        case graphics::BufferStorageMode::sharedStorage:
            if constexpr (ARCH_ARM64) {
                options |= MTL::ResourceStorageModeShared;
            } else {
                options |= MTL::ResourceStorageModeManaged;
            }
        case graphics::BufferStorageMode::privateStorage: options |= MTL::ResourceStorageModePrivate; break;
        default:
            ASSERT(storageMode == graphics::BufferStorageMode::sharedStorage,
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
    explicit Buffer(PtrView<MTL::Buffer> buffer);
    Buffer(PtrView<Device> device, graphics::BufferDescriptor const& descriptor);
    ~Buffer() noexcept override = default;

    [[nodiscard]] size_t size() const { return static_cast<size_t>(mtlBuffer()->length()); }
    [[nodiscard]] uint64 gpuAddress() const override { return static_cast<uint64>(mtlBuffer()->gpuAddress()); }

    [[nodiscard]] MTL::Buffer* mtlBuffer() const noexcept { return static_cast<MTL::Buffer*>(m_resource.get()); }
};

} // namespace mini::metal4
