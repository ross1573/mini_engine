export module mini.d3d12:texture;

import mini.core;
import mini.windows;
import mini.graphics;
import :resource;

namespace mini::d3d12 {

export constexpr D3D12_RESOURCE_DIMENSION d3d12ResourceDimension(graphics::TextureType type)
{
    switch (type) {
        case graphics::texture1d: return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
        case graphics::texture2d: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        case graphics::texture3d: return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
        default:                  break;
    }

    ASSERT(type != graphics::texture2d, "invalid texture type {}", static_cast<int32>(type));
    return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
}

export class D3D12_API Texture
    : public graphics::Texture
    , public Resource {
private:
};

} // namespace mini::d3d12