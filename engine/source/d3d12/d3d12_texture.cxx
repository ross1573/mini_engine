export module mini.d3d12:texture;

import mini.core;
import mini.windows;
import mini.graphics;
import :resource;

namespace mini::d3d12 {

export D3D12_API constexpr D3D12_RESOURCE_DIMENSION d3d12ResourceDimension(graphics::TextureType type) noexcept
{
    switch (type) {
        case graphics::TextureType::texture1d:
        case graphics::TextureType::texture1dArray:   return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
        case graphics::TextureType::texture2d:
        case graphics::TextureType::texture2dArray:
        case graphics::TextureType::textureCube:
        case graphics::TextureType::textureCubeArray: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        case graphics::TextureType::texture3d:        return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
    }

    ASSERT(type == graphics::TextureType::texture2d, "invalid texture type. type: {}.", static_cast<byte>(type));
    return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
}

export D3D12_API constexpr D3D12_RESOURCE_FLAGS d3d12ResourceFlags(graphics::TextureUsage usage) noexcept
{
    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

    if ((static_cast<byte>(usage) & static_cast<byte>(graphics::TextureUsage::shaderWrite)) != 0) {
        flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }
    if ((static_cast<byte>(usage) & static_cast<byte>(graphics::TextureUsage::renderTarget)) != 0) {
        flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    }

    return flags;
}

export D3D12_API constexpr D3D12_RESOURCE_DESC d3d12ResourceDescriptor(
    graphics::TextureDescriptor const& descriptor) noexcept
{
    ASSERT(descriptor.mipmapLevel > 0, "invalid mipmap count. {}", descriptor.mipmapLevel);

    D3D12_RESOURCE_DESC desc;
    desc.Dimension = d3d12ResourceDimension(descriptor.type);
    desc.MipLevels = descriptor.mipmapLevel;
    desc.Format = dxgiFormat(descriptor.pixelFormat);
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = d3d12ResourceFlags(descriptor.usage);

    switch (descriptor.type) {
        case graphics::TextureType::texture1d:
            desc.Width = descriptor.width;
            desc.Height = 1;
            desc.DepthOrArraySize = 1;
            ASSERT(descriptor.height == 1, "invalid height on texture1d. height: {}", descriptor.height);
            ASSERT(descriptor.depth == 1, "invalid depth on texture1d. depth: {}", descriptor.depth);
            break;

        case graphics::TextureType::texture1dArray:
            desc.Width = descriptor.width;
            desc.Height = 1;
            desc.DepthOrArraySize = descriptor.depth;
            ASSERT(descriptor.height == 1, "invalid height on texture1dArray. height: {}", descriptor.height);
            break;

        case graphics::TextureType::texture2d:
            desc.Width = descriptor.width;
            desc.Height = static_cast<UINT>(descriptor.height);
            desc.DepthOrArraySize = 1;
            ASSERT(descriptor.depth == 1, "invalid depth on texture2d. depth: {}", descriptor.depth);
            break;

        case graphics::TextureType::textureCube:
            desc.Width = descriptor.width;
            desc.Height = static_cast<UINT>(descriptor.height);
            desc.DepthOrArraySize = 6;
            ASSERT(descriptor.depth == 1, "invalid depth on textureCube. depth: {}", descriptor.depth);
            break;

        case graphics::TextureType::textureCubeArray:
            desc.Width = descriptor.width;
            desc.Height = static_cast<UINT>(descriptor.height);
            desc.DepthOrArraySize = 6 * descriptor.depth;
            break;

        case graphics::TextureType::texture2dArray:
        case graphics::TextureType::texture3d:
            desc.Width = descriptor.width;
            desc.Height = static_cast<UINT>(descriptor.height);
            desc.DepthOrArraySize = descriptor.depth;
            break;
    }

    return desc;
}

export class D3D12_API Texture final
    : public graphics::Texture
    , public Resource {
public:
    constexpr Texture() noexcept = default;
    explicit constexpr Texture(PtrView<ID3D12Resource> texture) noexcept;
    Texture(graphics::TextureDescriptor const& descriptor);

    [[nodiscard]] uint64 gpuAddress() const;
};

constexpr Texture::Texture(PtrView<ID3D12Resource> texture) noexcept
    : d3d12::Resource(texture)
{
}

uint64 Texture::gpuAddress() const
{
    return m_resource->GetGPUVirtualAddress();
}

} // namespace mini::d3d12