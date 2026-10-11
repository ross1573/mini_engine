module mini.metal4;

import :device;
import :buffer;
import :texture;

namespace mini::metal4 {

Buffer::Buffer(PtrView<MTL::Buffer> buffer)
{
    ASSERT(buffer);

    m_resource = SharedPtr<MTL::Buffer>(buffer.get());
}

Buffer::Buffer(PtrView<Device> device, graphics::BufferDescriptor const& descriptor)
{
    ASSERT(device);
    ASSERT(descriptor.length > 0, "invalid buffer size {}.", descriptor.length);

    MTL::ResourceOptions options = mtlBufferResourceOptions(descriptor.cacheMode, descriptor.storageMode);
    MTL::Resource* resource = device->mtlDevice()->newBuffer(descriptor.length, options);
    ENSURE(resource, "failed to allocate buffer") {
        return;
    }

    m_resource = transfer(resource);
}

SharedPtr<MTL::TextureDescriptor> mtlTextureDescriptor(graphics::TextureDescriptor const& descriptor)
{
    SharedPtr<MTL::TextureDescriptor> desc = transfer(MTL::TextureDescriptor::alloc());
    ENSURE(desc, "failed to allocate MTL::TextureDescriptor") {
        return SharedPtr<MTL::TextureDescriptor>{};
    }

    desc->init();
    desc->setWidth(descriptor.width);
    desc->setHeight(descriptor.height);
    desc->setDepth(descriptor.depth);
    desc->setMipmapLevelCount(descriptor.mipmapLevel);
    desc->setSampleCount(1);
    desc->setPixelFormat(mtlPixelFormat(descriptor.pixelFormat));
    desc->setTextureType(mtlTextureType(descriptor.type));
    desc->setUsage(mtlTextureUsage(descriptor.usage));
    desc->setSwizzle(mtlTextureSwizzleChannels(descriptor.swizzle));
    desc->setResourceOptions(mtlTextureResourceOptions(descriptor.cacheMode, descriptor.storageMode));
    return desc;
}

Texture::Texture(PtrView<MTL::Texture> texture) noexcept
{
    ASSERT(texture);

    m_resource = SharedPtr<MTL::Texture>(texture.get());
}

Texture::Texture(PtrView<Device> device, graphics::TextureDescriptor const& descriptor)
{
    ASSERT(device);

    SharedPtr<MTL::TextureDescriptor> desc = mtlTextureDescriptor(descriptor);
    MTL::Resource* resource = device->mtlDevice()->newTexture(desc.get());
    ENSURE(resource, "failed to allocate texture") {
        return;
    }

    m_resource = transfer(resource);
}

} // namespace mini::metal4