module;

#include <Metal/MTLTexture.hpp>

export module mini.metal4:texture;

import mini.core;
import mini.graphics;
import :resource;

export namespace MTL {

using MTL::Texture;
using MTL::TextureCompressionType;
using MTL::TextureDescriptor;
using MTL::TextureSwizzle;
using MTL::TextureType;
using MTL::TextureUsage;
using MTL::TextureUsagePixelFormatView;
using MTL::TextureUsageRenderTarget;
using MTL::TextureUsageShaderAtomic;
using MTL::TextureUsageShaderRead;
using MTL::TextureUsageShaderWrite;
using MTL::TextureUsageUnknown;
using MTL::TextureViewDescriptor;

} // namespace MTL

namespace mini::metal4 {

class Device;

} // namespace mini::metal4

namespace mini::metal4 {

export METAL4_API constexpr MTL::TextureType mtlTextureType(graphics::TextureType type) noexcept
{
    switch (type) {
        case graphics::TextureType::texture1d:        return MTL::TextureType1D;
        case graphics::TextureType::texture1dArray:   return MTL::TextureType1DArray;
        case graphics::TextureType::texture2d:        return MTL::TextureType2D;
        case graphics::TextureType::texture2dArray:   return MTL::TextureType2DArray;
        case graphics::TextureType::texture3d:        return MTL::TextureType3D;
        case graphics::TextureType::textureCube:      return MTL::TextureTypeCube;
        case graphics::TextureType::textureCubeArray: return MTL::TextureTypeCubeArray;
    }

    ASSERT(type == graphics::TextureType::texture2d, "invalid texture type {}.", static_cast<byte>(type));
    return MTL::TextureType2D;
}

export METAL4_API constexpr MTL::TextureUsage mtlTextureUsage(graphics::TextureUsage usage) noexcept
{
    MTL::TextureUsage result = MTL::TextureUsageUnknown;

    if ((static_cast<byte>(usage) & static_cast<byte>(graphics::TextureUsage::shaderRead)) != 0) {
        result |= MTL::TextureUsageShaderRead;
    }
    if ((static_cast<byte>(usage) & static_cast<byte>(graphics::TextureUsage::shaderWrite)) != 0) {
        result |= MTL::TextureUsageShaderWrite;
    }
    if ((static_cast<byte>(usage) & static_cast<byte>(graphics::TextureUsage::renderTarget)) != 0) {
        result |= MTL::TextureUsageRenderTarget;
    }

    return result;
}

export METAL4_API constexpr MTL::TextureSwizzle mtlTextureSwizzle(graphics::TextureSwizzle::Channel channel) noexcept
{
    switch (channel) {
        case graphics::TextureSwizzle::Channel::zero:  return MTL::TextureSwizzleZero;
        case graphics::TextureSwizzle::Channel::one:   return MTL::TextureSwizzleOne;
        case graphics::TextureSwizzle::Channel::red:   return MTL::TextureSwizzleRed;
        case graphics::TextureSwizzle::Channel::green: return MTL::TextureSwizzleGreen;
        case graphics::TextureSwizzle::Channel::blue:  return MTL::TextureSwizzleBlue;
        case graphics::TextureSwizzle::Channel::alpha: return MTL::TextureSwizzleAlpha;
    }

    ASSERT(channel == graphics::TextureSwizzle::Channel::zero,
           "invalid texture swizzel channel {}.",
           static_cast<byte>(channel));
    return MTL::TextureSwizzleZero;
}

export METAL4_API constexpr MTL::TextureSwizzleChannels mtlTextureSwizzleChannels(
    graphics::TextureSwizzle swizzle) noexcept
{
    return MTL::TextureSwizzleChannels{
        mtlTextureSwizzle(swizzle.red),
        mtlTextureSwizzle(swizzle.green),
        mtlTextureSwizzle(swizzle.blue),
        mtlTextureSwizzle(swizzle.alpha),
    };
}

export METAL4_API constexpr MTL::ResourceOptions mtlTextureResourceOptions(graphics::TextureStorage storage) noexcept
{
    MTL::ResourceOptions options{0};

    switch (storage) {
        default:
            ASSERT(storage == graphics::TextureStorage::gpuLocal,
                   "invalid resource storage {}.",
                   static_cast<byte>(storage));
            [[fallthrough]];

        case graphics::TextureStorage::gpuLocal:
            options |= MTL::ResourceCPUCacheModeDefaultCache;
            options |= MTL::ResourceStorageModePrivate;
            break;

        case graphics::TextureStorage::cpuWrite:
            options |= MTL::ResourceCPUCacheModeWriteCombined;
            if constexpr (ARCH_ARM64) {
                options |= MTL::ResourceStorageModeShared;
            } else {
                options |= MTL::ResourceStorageModeManaged;
            }
            break;

        case graphics::TextureStorage::cpuRead:
            options |= MTL::ResourceCPUCacheModeDefaultCache;
            if constexpr (ARCH_ARM64) {
                options |= MTL::ResourceStorageModeShared;
            } else {
                options |= MTL::ResourceStorageModeManaged;
            }
            break;

        case graphics::TextureStorage::transient:
            options |= MTL::ResourceCPUCacheModeDefaultCache;
            options |= MTL::ResourceStorageModeMemoryless;
            break;
    }

    options |= MTL::ResourceHazardTrackingModeUntracked;
    return options;
}

export METAL4_API SharedPtr<MTL::TextureDescriptor> mtlTextureDescriptor(graphics::TextureDescriptor const& descriptor,
                                                                         graphics::TextureStorage storage);

export class METAL4_API Texture
    : public graphics::Texture
    , public Resource {
public:
    constexpr Texture() noexcept = default;
    explicit constexpr Texture(PtrView<MTL::Texture> texture) noexcept;
    Texture(PtrView<Device> device, graphics::TextureDescriptor const& descriptor, graphics::TextureStorage storage);

    [[nodiscard]] uint64 gpuAddress() const override;
    [[nodiscard]] MTL::Texture* mtlTexture() const noexcept;
};

constexpr Texture::Texture(PtrView<MTL::Texture> texture) noexcept
    : metal4::Resource(staticCast<MTL::Resource>(texture))
{
}

uint64 Texture::gpuAddress() const
{
    return static_cast<uint64>(mtlTexture()->gpuResourceID()._impl);
}

MTL::Texture* Texture::mtlTexture() const noexcept
{
    return static_cast<MTL::Texture*>(m_resource.get());
}

} // namespace mini::metal4