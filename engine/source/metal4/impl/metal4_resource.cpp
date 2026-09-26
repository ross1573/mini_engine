module;
#include <Metal/MTLTexture.hpp>
module mini.metal4;

import :buffer;
import :texture;

namespace mini::metal4 {

Buffer::Buffer(Device* device, graphics::BufferDescriptor const& descriptor)
{
    ASSERT(device);
    ASSERT(descriptor.length > 0, "invalid buffer size {}.", descriptor.length);

    MTL::ResourceOptions options = MTLBufferResourceOptions(descriptor.cacheMode, descriptor.storageMode);
    m_resource = TransferShared(device->MTLDevice()->newBuffer(descriptor.length, options));
}

SharedPtr<MTL::TextureDescriptor> MTLTextureDescriptor(graphics::TextureDescriptor const& descriptor)
{
    SharedPtr<MTL::TextureDescriptor> desc = TransferShared(MTL::TextureDescriptor::alloc());
    ENSURE(desc, "failed to allocate MTL::TextureDescriptor") {
        return SharedPtr<MTL::TextureDescriptor>{};
    }

    desc->init();
    desc->setWidth(descriptor.width);
    desc->setHeight(descriptor.height);
    desc->setDepth(descriptor.depth);
    desc->setMipmapLevelCount(descriptor.mipmapLevel);
    desc->setSampleCount(1);
    desc->setPixelFormat(MTLPixelFormat(descriptor.pixelFormat));
    desc->setTextureType(MTLTextureType(descriptor.type));
    desc->setUsage(MTLTextureUsage(descriptor.usage));
    desc->setSwizzle(MTLTextureSwizzleChannels(descriptor.swizzle));
    desc->setResourceOptions(MTLTextureResourceOptions(descriptor.cacheMode, descriptor.storageMode));
    return desc;
}

Texture::Texture(MTL::Texture* texture) noexcept
{
    ASSERT(texture);
    m_resource = SharedPtr<MTL::Texture>(texture);
}

Texture::Texture(Device* device, graphics::TextureDescriptor const& descriptor)
{
    SharedPtr<MTL::TextureDescriptor> desc = MTLTextureDescriptor(descriptor);
    ENSURE(desc, "invalid descriptor") {
        return;
    }

    m_resource = TransferShared<MTL::Resource>(device->MTLDevice()->newTexture(desc.Get()));
}

} // namespace mini::metal4