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

SharedPtr<MTL::TextureDescriptor> mtlTextureDescriptor(graphics::Texture2DDescriptor const& descriptor,
                                                       graphics::TextureStorage storage)
{
    SharedPtr<MTL::TextureDescriptor> desc = transfer(MTL::TextureDescriptor::alloc());
    ENSURE(desc, "failed to allocate MTL::TextureDescriptor") {
        return SharedPtr<MTL::TextureDescriptor>{};
    }

    desc->init();
    desc->setWidth(descriptor.width);
    desc->setHeight(descriptor.height);
    desc->setMipmapLevelCount(descriptor.mipmap);
    desc->setSampleCount(1);
    desc->setPixelFormat(mtlPixelFormat(descriptor.pixelFormat));
    desc->setTextureType(MTL::TextureType2D);
    desc->setUsage(mtlTextureUsage(descriptor.usage));
    desc->setResourceOptions(mtlTextureResourceOptions(storage));
    return desc;
}

Texture::Texture(PtrView<MTL::Texture> texture) noexcept
{
    ASSERT(texture);

    m_resource = SharedPtr<MTL::Texture>(texture.get());
}

Texture2D::Texture2D(PtrView<Device> device,
                     graphics::Texture2DDescriptor const& descriptor,
                     graphics::TextureStorage storage)
{
    ASSERT(device);

    SharedPtr<MTL::TextureDescriptor> desc = mtlTextureDescriptor(descriptor, storage);
    MTL::Resource* resource = device->mtlDevice()->newTexture(desc.get());
    ENSURE(resource, "failed to allocate texture") {
        return;
    }

    m_resource = transfer(resource);
}

} // namespace mini::metal4