export module mini.metal4:texture;

import mini.core;
import mini.graphics;
import :resource;

namespace mini::metal4 {

export class METAL4_API Texture : public Resource<MTL::Texture> {
private:
    typedef Resource<MTL::Texture> Base;

public:
    Texture() noexcept = default;
    Texture(MTL::Texture*) noexcept;
    Texture(MTL::Texture*, StringView);

    uint64 GpuAddress() const;
};

Texture::Texture(MTL::Texture* texture) noexcept
    : Base(texture)
{
}

Texture::Texture(MTL::Texture* texture, StringView name)
    : Base(texture, name)
{
}

uint64 Texture::GpuAddress() const
{
    return static_cast<uint64>(m_resource->gpuResourceID()._impl);
}

} // namespace mini::metal4