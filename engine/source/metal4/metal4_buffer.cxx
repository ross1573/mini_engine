export module mini.metal4:buffer;

import mini.core;
import mini.graphics;
import :resource;

namespace mini::metal4 {

export class METAL4_API Buffer : public Resource<MTL::Buffer> {
private:
    typedef Resource<MTL::Buffer> Base;

public:
    Buffer() noexcept = default;
    Buffer(MTL::Buffer*) noexcept;
    Buffer(MTL::Buffer*, StringView);

    SizeT Size() const;
    uint64 GpuAddress() const;
};

Buffer::Buffer(MTL::Buffer* buffer) noexcept
    : Base(buffer)
{
}

Buffer::Buffer(MTL::Buffer* buffer, StringView name)
    : Base(buffer, name)
{
}

SizeT Buffer::Size() const
{
    return static_cast<SizeT>(m_resource->length());
}

uint64 Buffer::GpuAddress() const
{
    return static_cast<uint64>(m_resource->gpuAddress());
}

} // namespace mini::metal4
