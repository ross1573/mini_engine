module mini.d3d12;

import :device;
import :resource;

namespace mini::d3d12 {

size_t Resource::capacity() const noexcept
{
    if (!valid()) {
        return 0;
    }

    ID3D12Device* device;
    ENSURE(m_resource->GetDevice(COM_ARG(&device))) {
        return 0;
    }

    D3D12_RESOURCE_DESC desc;
    D3D12_RESOURCE_ALLOCATION_INFO allocInfo;

#if PLATFORM_MINGW
    ignore = m_resource->GetDesc(&desc);
    ignore = device->GetResourceAllocationInfo(&allocInfo, 0, 1, &desc);
#else
    desc = m_resource->GetDesc();
    allocInfo = device->GetResourceAllocationInfo(0, 1, &desc);
#endif
    return static_cast<size_t>(allocInfo.SizeInBytes);
}

} // namespace mini::d3d12