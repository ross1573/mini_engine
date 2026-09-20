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

export class METAL4_API Buffer : public Resource<MTL::Buffer> {
private:
    typedef Resource<MTL::Buffer> Base;

public:
    Buffer() noexcept = default;
    explicit Buffer(MTL::Buffer* buffer) noexcept;
    Buffer(MTL::Buffer* buffer, StringView name);

    [[nodiscard]] size_t Size() const;
    [[nodiscard]] uint64 GpuAddress() const;
};

Buffer::Buffer(MTL::Buffer* buffer) noexcept
    : Base(buffer)
{
}

Buffer::Buffer(MTL::Buffer* buffer, StringView name)
    : Base(buffer, name)
{
}

size_t Buffer::Size() const
{
    return static_cast<size_t>(m_resource->length());
}

uint64 Buffer::GpuAddress() const
{
    return static_cast<uint64>(m_resource->gpuAddress());
}

} // namespace mini::metal4
