export module mini.metal4:resource;

import mini.core;
import mini.apple;
import mini.graphics;

namespace mini::metal4 {

export template <DerivedFromT<MTL::Resource> T>
class METAL4_API Resource {
public:
    typedef T ResourceValue;
    typedef T* ResourcePointer;

protected:
    SharedPtr<ResourceValue> m_resource;
    SharedPtr<NS::String> m_name;

public:
    Resource(ResourcePointer) noexcept;
    Resource(ResourcePointer, StringView);

    bool Valid() const noexcept;

    void SetName(StringView);

    SizeT Capacity() const;
    String Name() const;
    ResourcePointer MetalResource() const noexcept;

private:
    Resource() noexcept = delete;
    Resource(Resource const&) = delete;
    Resource(Resource&&) = delete;

    Resource& operator=(Resource const&) = delete;
    Resource& operator=(Resource&&) = delete;
};

template <DerivedFromT<MTL::Resource> T>
Resource<T>::Resource(ResourcePointer resource) noexcept
    : m_resource(TransferShared(resource))
{
    ASSERT(m_resource.Valid());
}

template <DerivedFromT<MTL::Resource> T>
Resource<T>::Resource(ResourcePointer resource, StringView name)
    : m_resource(TransferShared(resource))
{
    ASSERT(m_resource.Valid());
    SetName(name);
}

template <DerivedFromT<MTL::Resource> T>
bool Resource<T>::Valid() const noexcept
{
    return m_resource.Valid();
}

template <DerivedFromT<MTL::Resource> T>
void Resource<T>::SetName(StringView name)
{
    NS::String* label = NS::String::alloc();
    ENSURE(label != nullptr, "failed to allocate NS::String") {
        return;
    }

    label = label->init(static_cast<void*>(const_cast<char*>(name.Data())),
                        static_cast<NS::UInteger>(name.Size()),
                        NS::StringEncoding::UTF8StringEncoding,
                        false);

    ENSURE(label != nullptr, "failed to initialize NS::String") {
        return;
    }

    m_resource->setLabel(label);
    m_name.Reset(label);
}

template <DerivedFromT<MTL::Resource> T>
SizeT Resource<T>::Capacity() const
{
    return static_cast<SizeT>(m_resource->allocatedSize());
}

template <DerivedFromT<MTL::Resource> T>
String Resource<T>::Name() const
{
    if (m_name.Valid() == false) {
        return String();
    }

    char const* raw = m_name->utf8String();
    SizeT len = static_cast<SizeT>(m_name->length());
    return String(raw, len);
}

template <DerivedFromT<MTL::Resource> T>
Resource<T>::ResourcePointer Resource<T>::MetalResource() const noexcept
{
    return m_resource.Get();
}

} // namespace mini::metal4