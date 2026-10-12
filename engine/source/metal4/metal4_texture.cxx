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
        case graphics::TextureType::textrue2dArray:   return MTL::TextureType2DArray;
        case graphics::TextureType::texture3d:        return MTL::TextureType3D;
        case graphics::TextureType::textureCube:      return MTL::TextureTypeCube;
        case graphics::TextureType::textureCubeArray: return MTL::TextureTypeCubeArray;
    }

    ASSERT(type == graphics::TextureType::texture2d, "invalid texture type {}.", static_cast<byte>(type));
    return MTL::TextureType2D;
}

export METAL4_API constexpr MTL::TextureUsage mtlTextureUsage(graphics::TextureUsage usage) noexcept
{
    switch (usage) {
        case graphics::TextureUsage::undefined:    return MTL::TextureUsageUnknown;
        case graphics::TextureUsage::shaderRead:   return MTL::TextureUsageShaderRead;
        case graphics::TextureUsage::shaderWrite:  return MTL::TextureUsageShaderWrite;
        case graphics::TextureUsage::renderTarget: return MTL::TextureUsageRenderTarget;
    }

    ASSERT(usage == graphics::TextureUsage::undefined, "invalid texture usage {}.", static_cast<byte>(usage));
    return MTL::TextureUsageUnknown;
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

export METAL4_API constexpr MTL::ResourceOptions mtlTextureResourceOptions(
    graphics::TextureCacheMode cacheMode,
    graphics::TextureStorageMode storageMode) noexcept
{
    MTL::ResourceOptions options{0};

    switch (cacheMode) {
        case graphics::TextureCacheMode::writeCombined: options |= MTL::ResourceCPUCacheModeWriteCombined; break;
        case graphics::TextureCacheMode::defaultCache:  break;
        default:
            ASSERT(cacheMode == graphics::TextureCacheMode::defaultCache,
                   "invalid resource cache mode {}.",
                   static_cast<byte>(cacheMode));
            break;
    }

    switch (storageMode) {
        case graphics::TextureStorageMode::sharedStorage:
            if constexpr (ARCH_ARM64) {
                options |= MTL::ResourceStorageModeShared;
            } else {
                options |= MTL::ResourceStorageModeManaged;
            }
        case graphics::TextureStorageMode::privateStorage: options |= MTL::ResourceStorageModePrivate; break;
        case graphics::TextureStorageMode::transient:      options |= MTL::ResourceStorageModeMemoryless; break;
        default:
            ASSERT(storageMode == graphics::TextureStorageMode::sharedStorage,
                   "invalid resource storage mode {}.",
                   static_cast<byte>(storageMode));
            break;
    }

    options |= MTL::ResourceHazardTrackingModeUntracked;
    return options;
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

export METAL4_API SharedPtr<MTL::TextureDescriptor> mtlTextureDescriptor(graphics::TextureDescriptor const& descriptor);

export class METAL4_API Texture
    : public graphics::Texture
    , public Resource {
public:
    explicit Texture(PtrView<MTL::Texture> texture) noexcept;
    Texture(PtrView<Device> device, graphics::TextureDescriptor const& descriptor);
    ~Texture() noexcept override = default;

    [[nodiscard]] uint64 gpuAddress() const override;

    [[nodiscard]] MTL::Texture* mtlTexture() const noexcept { return static_cast<MTL::Texture*>(m_resource.get()); }
};

uint64 Texture::gpuAddress() const
{
    return static_cast<uint64>(mtlTexture()->gpuResourceID()._impl);
}

} // namespace mini::metal4