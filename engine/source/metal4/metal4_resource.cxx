export module mini.metal4:resource;

import mini.core;
import mini.apple;
import mini.graphics;
import :common;

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
    explicit Resource(nullptr_t) noexcept;
    explicit Resource(ResourcePointer) noexcept;
    explicit Resource(ResourcePointer, StringView);

    void SetName(StringView);

    bool Valid() const noexcept;
    size_t Capacity() const;
    String Name() const;

    ResourcePointer MTLResource() const noexcept;
    ResourcePointer operator->() const noexcept;
};

template <DerivedFromT<MTL::Resource> T>
Resource<T>::Resource(nullptr_t) noexcept
    : m_resource(nullptr)
{
}

template <DerivedFromT<MTL::Resource> T>
Resource<T>::Resource(ResourcePointer resource) noexcept
    : m_resource(TransferShared(resource))
{
    ASSERT(m_resource.Valid());
}

template <DerivedFromT<MTL::Resource> T>
Resource<T>::Resource(ResourcePointer resource, StringView name)
    : m_resource(TransferShared(resource))
    , m_name(SetLabel(m_resource, name))
{
    ASSERT(m_resource.Valid());
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

    m_name = SetLabel(m_resource, name);
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
Resource<T>::ResourcePointer Resource<T>::MTLResource() const noexcept
{
    return m_resource.Get();
}

template <DerivedFromT<MTL::Resource> T>
Resource<T>::ResourcePointer Resource<T>::operator->() const noexcept
{
    return m_resource.operator->();
}

} // namespace mini::metal4