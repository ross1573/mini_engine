module;

#include <Metal/MTLGpuAddress.hpp>
#include <Metal/MTLResource.hpp>

export module mini.metal4:resource;

import mini.core;
import mini.apple;
import mini.graphics;
import :common;

export namespace MTL {

using MTL::BufferSparseTier;
using MTL::CPUCacheMode;
using MTL::GPUAddress;
using MTL::HazardTrackingMode;
using MTL::PurgeableState;
using MTL::Resource;
using MTL::ResourceCPUCacheModeDefaultCache;
using MTL::ResourceCPUCacheModeWriteCombined;
using MTL::ResourceHazardTrackingModeDefault;
using MTL::ResourceHazardTrackingModeTracked;
using MTL::ResourceHazardTrackingModeUntracked;
using MTL::ResourceOptions;
using MTL::ResourceStorageModeManaged;
using MTL::ResourceStorageModeMemoryless;
using MTL::ResourceStorageModePrivate;
using MTL::ResourceStorageModeShared;
using MTL::SparsePageSize;
using MTL::StorageMode;
using MTL::TextureSparseTier;

} // namespace MTL

namespace mini::metal4 {

export class METAL4_API Resource : public graphics::Resource {
protected:
    SharedPtr<MTL::Resource> m_resource;

public:
    void SetName(StringView name) { m_resource->setLabel(ToNSString(name).Get()); }

    [[nodiscard]] bool Valid() const noexcept { return m_resource.Valid(); }
    [[nodiscard]] size_t Capacity() const { return static_cast<size_t>(m_resource->allocatedSize()); }
    [[nodiscard]] String Name() const { return ToString(m_resource->label()); }

    [[nodiscard]] MTL::Resource* MTLResource() const noexcept { return m_resource.Get(); }
};

} // namespace mini::metal4