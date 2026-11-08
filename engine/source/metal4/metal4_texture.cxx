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

export METAL4_API SharedPtr<MTL::TextureDescriptor> mtlTextureDescriptor(
    graphics::Texture2DDescriptor const& descriptor,
    graphics::TextureStorage storage);

export class METAL4_API Texture
    : public graphics::Texture
    , public Resource {
public:
    explicit Texture(PtrView<MTL::Texture> texture) noexcept;
    Texture() noexcept = default;
    ~Texture() noexcept override = default;

    [[nodiscard]] uint64 gpuAddress() const override;
    [[nodiscard]] MTL::Texture* mtlTexture() const noexcept;
};

uint64 Texture::gpuAddress() const
{
    return static_cast<uint64>(mtlTexture()->gpuResourceID()._impl);
}

MTL::Texture* Texture::mtlTexture() const noexcept
{
    return static_cast<MTL::Texture*>(m_resource.get());
}

export class METAL4_API Texture2D : public Texture {
public:
    Texture2D(PtrView<Device> device,
              graphics::Texture2DDescriptor const& descriptor,
              graphics::TextureStorage storage);

    ~Texture2D() noexcept override = default;
};

} // namespace mini::metal4