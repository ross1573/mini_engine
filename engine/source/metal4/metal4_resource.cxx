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
    String m_name;

public:
    Resource() noexcept = default;
    Resource(Resource const&) noexcept = default;
    Resource(Resource&&) noexcept = default;
    Resource(ResourcePointer) noexcept;
    Resource(ResourcePointer, StringView);

    bool Valid() const noexcept;

    void SetName(StringView);

    size_t Capacity() const;
    String Name() const;
    ResourcePointer MetalResource() const noexcept;

    Resource& operator=(Resource const&) noexcept = default;
    Resource& operator=(Resource&&) noexcept = default;
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
    if (m_name == name) {
        return;
    }

    SharedPtr<NS::String> label = ToNSString(name);
    if (label != nullptr) {
        m_resource->setLabel(label.Get());
        m_name = name;
    }
}

template <DerivedFromT<MTL::Resource> T>
size_t Resource<T>::Capacity() const
{
    return static_cast<size_t>(m_resource->allocatedSize());
}

template <DerivedFromT<MTL::Resource> T>
String Resource<T>::Name() const
{
    return m_name;
}

template <DerivedFromT<MTL::Resource> T>
Resource<T>::ResourcePointer Resource<T>::MetalResource() const noexcept
{
    return m_resource.Get();
}

} // namespace mini::metal4