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

SharedPtr<MTL::TextureDescriptor> mtlTextureDescriptor(graphics::TextureDescriptor const& descriptor,
                                                       graphics::TextureStorage storage)
{
    SharedPtr<MTL::TextureDescriptor> desc = transfer(MTL::TextureDescriptor::alloc());
    ENSURE(desc, "failed to allocate MTL::TextureDescriptor") {
        return SharedPtr<MTL::TextureDescriptor>{};
    }

    desc->init();
    desc->setTextureType(mtlTextureType(descriptor.type));
    desc->setPixelFormat(mtlPixelFormat(descriptor.pixelFormat));
    desc->setMipmapLevelCount(descriptor.mipmapLevel);
    desc->setSampleCount(1);
    desc->setResourceOptions(mtlTextureResourceOptions(storage));
    desc->setUsage(mtlTextureUsage(descriptor.usage));

    switch (descriptor.type) {
        case graphics::TextureType::texture1d:
            desc->setWidth(descriptor.width);
            desc->setHeight(1);
            desc->setArrayLength(1);
            ASSERT(descriptor.height == 1, "invalid height on texture1d. height: {}", descriptor.height);
            ASSERT(descriptor.depth == 1, "invalid depth on texture1d. depth: {}", descriptor.depth);
            break;

        case graphics::TextureType::texture1dArray:
            desc->setWidth(descriptor.width);
            desc->setHeight(1);
            desc->setArrayLength(descriptor.depth);
            ASSERT(descriptor.depth == 1, "invalid depth on texture1d. depth: {}", descriptor.depth);
            break;

        case graphics::TextureType::texture2d:
            desc->setWidth(descriptor.width);
            desc->setHeight(descriptor.height);
            desc->setArrayLength(1);
            ASSERT(descriptor.depth == 1, "invalid depth on texture2d. depth: {}", descriptor.depth);
            break;

        case graphics::TextureType::textureCube:
            desc->setWidth(descriptor.width);
            desc->setHeight(descriptor.height);
            desc->setArrayLength(1);
            ASSERT(descriptor.depth == 1, "invalid depth on textureCube. depth: {}", descriptor.depth);
            break;

        case graphics::TextureType::texture2dArray:
        case graphics::TextureType::textureCubeArray:
            desc->setWidth(descriptor.width);
            desc->setHeight(descriptor.height);
            desc->setArrayLength(descriptor.depth);
            break;

        case graphics::TextureType::texture3d: desc->setArrayLength(1); break;
    }

    return desc;
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