module;

#include <Metal/MTLTexture.hpp>

export module mini.metal4:texture;

import mini.core;
import mini.graphics;
import :resource;
import :device;

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

export METAL4_API constexpr MTL::TextureType MTLTextureType(graphics::TextureType type) noexcept
{
    switch (type) {
        case graphics::TextureType::Texture1D:        return MTL::TextureType1D;
        case graphics::TextureType::Texture1DArray:   return MTL::TextureType1DArray;
        case graphics::TextureType::Texture2D:        return MTL::TextureType2D;
        case graphics::TextureType::Textrue2DArray:   return MTL::TextureType2DArray;
        case graphics::TextureType::Texture3D:        return MTL::TextureType3D;
        case graphics::TextureType::TextureCube:      return MTL::TextureTypeCube;
        case graphics::TextureType::TextureCubeArray: return MTL::TextureTypeCubeArray;
    }

    ASSERT(type == graphics::TextureType::Texture2D, "invalid texture type {}.", static_cast<byte>(type));
    return MTL::TextureType2D;
}

export METAL4_API constexpr MTL::TextureUsage MTLTextureUsage(graphics::TextureUsage usage) noexcept
{
    switch (usage) {
        case graphics::TextureUsage::Undefined:    return MTL::TextureUsageUnknown;
        case graphics::TextureUsage::ShaderRead:   return MTL::TextureUsageShaderRead;
        case graphics::TextureUsage::ShaderWrite:  return MTL::TextureUsageShaderWrite;
        case graphics::TextureUsage::RenderTarget: return MTL::TextureUsageRenderTarget;
    }

    ASSERT(usage == graphics::TextureUsage::Undefined, "invalid texture usage {}.", static_cast<byte>(usage));
    return MTL::TextureUsageUnknown;
}

export METAL4_API constexpr MTL::TextureSwizzle MTLTextureSwizzle(graphics::TextureSwizzle::Channel channel) noexcept
{
    switch (channel) {
        case graphics::TextureSwizzle::Channel::Zero:  return MTL::TextureSwizzleZero;
        case graphics::TextureSwizzle::Channel::One:   return MTL::TextureSwizzleOne;
        case graphics::TextureSwizzle::Channel::Red:   return MTL::TextureSwizzleRed;
        case graphics::TextureSwizzle::Channel::Green: return MTL::TextureSwizzleGreen;
        case graphics::TextureSwizzle::Channel::Blue:  return MTL::TextureSwizzleBlue;
        case graphics::TextureSwizzle::Channel::Alpha: return MTL::TextureSwizzleAlpha;
    }

    ASSERT(channel == graphics::TextureSwizzle::Channel::Zero,
           "invalid texture swizzel channel {}.",
           static_cast<byte>(channel));
    return MTL::TextureSwizzleZero;
}

export METAL4_API constexpr MTL::ResourceOptions MTLTextureResourceOptions(
    graphics::TextureCacheMode cacheMode,
    graphics::TextureStorageMode storageMode) noexcept
{
    MTL::ResourceOptions options{0};

    switch (cacheMode) {
        case graphics::TextureCacheMode::WriteCombined: options |= MTL::ResourceCPUCacheModeWriteCombined; break;
        case graphics::TextureCacheMode::Default:       break;
        default:
            ASSERT(cacheMode == graphics::TextureCacheMode::Default,
                   "invalid resource cache mode {}.",
                   static_cast<byte>(cacheMode));
            break;
    }

    switch (storageMode) {
        case graphics::TextureStorageMode::Shared:
            if constexpr (ARCH_ARM64) {
                options |= MTL::ResourceStorageModeShared;
            } else {
                options |= MTL::ResourceStorageModeManaged;
            }
        case graphics::TextureStorageMode::Private:   options |= MTL::ResourceStorageModePrivate; break;
        case graphics::TextureStorageMode::Transient: options |= MTL::ResourceStorageModeMemoryless; break;
        default:
            ASSERT(storageMode == graphics::TextureStorageMode::Shared,
                   "invalid resource storage mode {}.",
                   static_cast<byte>(storageMode));
            break;
    }

    options |= MTL::ResourceHazardTrackingModeUntracked;
    return options;
}

export METAL4_API constexpr MTL::TextureSwizzleChannels MTLTextureSwizzleChannels(
    graphics::TextureSwizzle swizzle) noexcept
{
    return MTL::TextureSwizzleChannels{
        MTLTextureSwizzle(swizzle.red),
        MTLTextureSwizzle(swizzle.green),
        MTLTextureSwizzle(swizzle.blue),
        MTLTextureSwizzle(swizzle.alpha),
    };
}

export METAL4_API SharedPtr<MTL::TextureDescriptor> MTLTextureDescriptor(graphics::TextureDescriptor const& descriptor);

export class METAL4_API Texture
    : public graphics::Texture
    , public Resource {
public:
    explicit Texture(MTL::Texture* texture) noexcept;
    Texture(Device* device, graphics::TextureDescriptor const& descriptor);
    ~Texture() noexcept override = default;

    [[nodiscard]] uint64 GpuAddress() const override;

    [[nodiscard]] MTL::Texture* MTLTexture() const noexcept { return static_cast<MTL::Texture*>(m_resource.Get()); }
};

uint64 Texture::GpuAddress() const
{
    return static_cast<uint64>(MTLTexture()->gpuResourceID()._impl);
}

} // namespace mini::metal4