module;

#include <Metal/Metal.hpp>

export module mini.metal4:resource;

import mini.core;
import mini.apple;
import mini.graphics;
import :common;

export namespace MTL {

using MTL::GPUAddress;
using MTL::Resource;

} // namespace MTL

namespace mini::metal4 {

export template <DerivedFromT<MTL::Resource> T>
class METAL4_API Resource {
public:
    typedef T ResourceValue;
    typedef T* ResourcePointer;

protected:
    SharedPtr<ResourceValue> m_resource;

public:
    Resource() noexcept = default;
    explicit Resource(nullptr_t) noexcept;
    explicit Resource(ResourcePointer resource) noexcept;
    explicit Resource(ResourcePointer resource, StringView name);

    void SetName(StringView name);

    [[nodiscard]] bool Valid() const noexcept;
    [[nodiscard]] size_t Capacity() const;
    [[nodiscard]] String Name() const;

    [[nodiscard]] ResourcePointer MTLResource() const noexcept;
    [[nodiscard]] ResourcePointer operator->() const noexcept;
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
    SharedPtr<NS::String> label = ToNSString(name);
    m_resource->setLabel(label.Get());
}

template <DerivedFromT<MTL::Resource> T>
size_t Resource<T>::Capacity() const
{
    return static_cast<size_t>(m_resource->allocatedSize());
}

template <DerivedFromT<MTL::Resource> T>
String Resource<T>::Name() const
{
    return ToString(m_resource->label());
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