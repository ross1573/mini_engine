#pragma once

#include "memory/shared_ptr.h"

namespace mini
{

template <NonRefT T> requires DerivedFromT<T, IUnknown>
class SharedPtr<T>
{
    template <NonRefT U>
    friend class SharedPtr;

public:
    typedef T Value;
    typedef T* Ptr;
    typedef T& Ref;

private:
    T* m_Ptr;

public:
    constexpr SharedPtr() noexcept;
    ~SharedPtr() noexcept;
    SharedPtr(SharedPtr const&) noexcept;
    constexpr SharedPtr(SharedPtr&&) noexcept;
    template <PtrConvertibleToT<T> U> SharedPtr(SharedPtr<U> const&) noexcept;
    template <PtrConvertibleToT<T> U> constexpr SharedPtr(SharedPtr<U>&&) noexcept;
    template <PtrConvertibleToT<T> U> explicit constexpr SharedPtr(U*, bool = true) noexcept;
    constexpr SharedPtr(NullptrT) noexcept;

    constexpr Ptr Get() const noexcept;
    constexpr bool IsValid() const noexcept;

    constexpr Ptr Detach() noexcept;
    constexpr void Swap(SharedPtr&) noexcept;
    void Reset() noexcept;
    template <PtrConvertibleToT<T> U> void Reset(U*) noexcept;

    template <NonRefT U> requires DerivedFromT<U, IUnknown>
    constexpr bool Equals(SharedPtr<U> const&) const noexcept
        requires EqualityComparableWithT<T*, U*>;

    constexpr Ptr operator->() const noexcept;
    constexpr Ref operator*() const noexcept;
    constexpr Ptr* operator&() noexcept;
    constexpr operator bool() const noexcept;
    constexpr operator Ptr() const noexcept;

    SharedPtr& operator=(SharedPtr const&) noexcept;
    constexpr SharedPtr& operator=(SharedPtr&&) noexcept;
    template <PtrConvertibleToT<T> U> SharedPtr& operator=(SharedPtr<U> const&) noexcept;
    template <PtrConvertibleToT<T> U> constexpr SharedPtr& operator=(SharedPtr<U>&&) noexcept;
};

template <NonRefT T> requires DerivedFromT<T, IUnknown>
[[force_inline]] constexpr SharedPtr<T>::SharedPtr() noexcept
    : m_Ptr(nullptr)
{
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
[[force_inline]] SharedPtr<T>::~SharedPtr() noexcept
{
    if (m_Ptr)
    {
        m_Ptr->Release();
        m_Ptr = nullptr;
    }
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
[[force_inline]] SharedPtr<T>::SharedPtr(SharedPtr const& other) noexcept
    : m_Ptr(other.m_Ptr)
{
    if (m_Ptr)
    {
        m_Ptr->AddRef();
    }
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
[[force_inline]] constexpr SharedPtr<T>::SharedPtr(SharedPtr&& other) noexcept
    : m_Ptr(other.m_Ptr)
{
    other.m_Ptr = nullptr;
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
template <PtrConvertibleToT<T> U>
[[force_inline]] SharedPtr<T>::SharedPtr(SharedPtr<U> const& other) noexcept
    : m_Ptr(static_cast<T*>(other.m_Ptr))
{
    if (m_Ptr)
    {
        m_Ptr->AddRef();
    }
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
template <PtrConvertibleToT<T> U>
[[force_inline]] constexpr SharedPtr<T>::SharedPtr(SharedPtr<U>&& other) noexcept
    : m_Ptr(static_cast<T*>(other.m_Ptr))
{
    other.m_Ptr = nullptr;
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
template <PtrConvertibleToT<T> U>
[[force_inline]] constexpr SharedPtr<T>::SharedPtr(U* ptr, bool addref) noexcept
    : m_Ptr(static_cast<T*>(ptr))
{
    if (m_Ptr && addref)
    {
        m_Ptr->AddRef();
    }
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
[[force_inline]] constexpr SharedPtr<T>::SharedPtr(NullptrT) noexcept
    : m_Ptr(nullptr)
{
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
[[force_inline]] constexpr SharedPtr<T>::Ptr SharedPtr<T>::Get() const noexcept
{
    return m_Ptr;
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
[[force_inline]] constexpr bool SharedPtr<T>::IsValid() const noexcept
{
    return m_Ptr != nullptr;
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
[[force_inline]] constexpr SharedPtr<T>::Ptr SharedPtr<T>::Detach() noexcept
{
    Ptr tmp = m_Ptr;
    m_Ptr = nullptr;
    return tmp;
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
[[force_inline]] constexpr void SharedPtr<T>::Swap(SharedPtr& o) noexcept
{
    Ptr tmp = m_Ptr;
    m_Ptr = o.m_Ptr;
    o.m_Ptr = tmp;
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
[[force_inline]] void SharedPtr<T>::Reset() noexcept
{
    if (m_Ptr)
    {
        m_Ptr->Release();
        m_Ptr = nullptr;
    }
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
template <PtrConvertibleToT<T> U>
[[force_inline]] void SharedPtr<T>::Reset(U* ptr) noexcept
{
    if (m_Ptr)
    {
        m_Ptr->Release();
    }

    m_Ptr = static_cast<T*>(ptr);
    m_Ptr->AddRef();
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
template <NonRefT U> requires DerivedFromT<U, IUnknown>
[[force_inline]] constexpr bool SharedPtr<T>::Equals(SharedPtr<U> const& other) const noexcept
    requires EqualityComparableWithT<T*, U*>
{
    return m_Ptr == other.m_Ptr;
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
[[force_inline]] constexpr SharedPtr<T>::Ptr SharedPtr<T>::operator->() const noexcept
{
    return m_Ptr;
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
[[force_inline]] constexpr SharedPtr<T>::Ref SharedPtr<T>::operator*() const noexcept
{
    return *m_Ptr;
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
[[force_inline]] constexpr SharedPtr<T>::Ptr* SharedPtr<T>::operator&() noexcept
{
    return &m_Ptr;
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
[[force_inline]] constexpr SharedPtr<T>::operator bool() const noexcept
{
    return m_Ptr != nullptr;
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
[[force_inline]] constexpr SharedPtr<T>::operator Ptr() const noexcept
{
    return m_Ptr;
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
[[force_inline]] SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr const& other) noexcept
{
    if (m_Ptr)
    {
        m_Ptr->Release();
    }

    m_Ptr = other.m_Ptr;
    m_Ptr->AddRef();

    return *this;
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
[[force_inline]] constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other) noexcept
{
    if (m_Ptr)
    {
        m_Ptr->Release();
    }

    m_Ptr = other.m_Ptr;
    other.m_Ptr = nullptr;

    return *this;
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
template <PtrConvertibleToT<T> U>
[[force_inline]] SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U> const& other) noexcept
{
    if (m_Ptr)
    {
        m_Ptr->Release();
    }

    m_Ptr = static_cast<T*>(other.m_Ptr);
    m_Ptr->AddRef();

    return *this;
}

template <NonRefT T> requires DerivedFromT<T, IUnknown>
template <PtrConvertibleToT<T> U>
[[force_inline]] constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U>&& other) noexcept
{
    if (m_Ptr)
    {
        m_Ptr->Release();
    }

    m_Ptr = static_cast<T*>(other.m_Ptr);
    other.m_Ptr = nullptr;

    return *this;
}

template <NonRefT T, UnbindedAllocatorTT AllocT, typename... Args>
constexpr SharedPtr<T> AllocateShared(AllocT const&, Args&&...)
    requires DerivedFromT<T, IUnknown>
{
    NEVER_CALLED("COM interface should not be constructed directly");
    return SharedPtr<T>();
}

template <NonRefT T, typename... Args>
constexpr SharedPtr<T> MakeShared(Args&&...)
    requires DerivedFromT<T, IUnknown>
{
    NEVER_CALLED("COM interface should not be constructed directly");
    return SharedPtr<T>();
}

template <NonRefT T, NonRefT U>
constexpr bool operator==(SharedPtr<T> const& l, U* r) noexcept
    requires DerivedFromT<T, IUnknown>&& DerivedFromT<U, IUnknown>
{
    return l.Get() == r;
}

template <NonRefT T, NonRefT U>
constexpr auto operator<=>(SharedPtr<T> const l, U* r) noexcept
    requires DerivedFromT<T, IUnknown>&& DerivedFromT<U, IUnknown>
{
    return l.Get <=> r;
}

template <NonRefT T, NonRefT U>
constexpr SharedPtr<T> StaticCast(SharedPtr<U> const& other) noexcept
    requires DerivedFromT<T, IUnknown>&& DerivedFromT<U, IUnknown>
{
    return SharedPtr<T>(static_cast<T*>(other.Get()));
}

template <NonRefT T, NonRefT U>
constexpr SharedPtr<T> StaticCast(SharedPtr<U>&& other)
    requires DerivedFromT<T, IUnknown>&& DerivedFromT<U, IUnknown>
{
    return SharedPtr<T>(static_cast<T*>(other.Detach()), false);
}

template <NonRefT T, NonRefT U>
constexpr SharedPtr<T> DynamicCast(SharedPtr<U> const& other) noexcept
    requires DerivedFromT<T, IUnknown>&& DerivedFromT<U, IUnknown>
{
    SharedPtr<T> result;
    if (other == nullptr) [[unlikely]]
    {
        return result;
    }

    other->QueryInterface(__uuidof(U), reinterpret_cast<void**>(&result));
    return result;
}

template <NonRefT T, NonRefT U>
constexpr SharedPtr<T> DynamicCast(SharedPtr<U>&& other) noexcept
    requires DerivedFromT<T, IUnknown>&& DerivedFromT<U, IUnknown>
{
    SharedPtr<T> result;
    if (other == nullptr) [[unlikely]]
    {
        return result;
    }

    other->QueryInterface(__uuidof(U), reinterpret_cast<void**>(&result));
    other->Reset();
    return result;
}

template <NonRefT T, NonRefT U>
constexpr SharedPtr<T> ConstCast(SharedPtr<U> const& other) noexcept
    requires DerivedFromT<T, IUnknown>&& DerivedFromT<U, IUnknown>
{
    return SharedPtr<T>(const_cast<T*>(other.Get()));
}

template <NonRefT T, NonRefT U>
constexpr SharedPtr<T> ConstCast(SharedPtr<U>&& other) noexcept
    requires DerivedFromT<T, IUnknown>&& DerivedFromT<U, IUnknown>
{
    return SharedPtr<T>(const_cast<T*>(other.Detach()), false);
}

template <NonRefT T, NonRefT U>
constexpr SharedPtr<T> ReinterpretCast(SharedPtr<U> const& other) noexcept
    requires DerivedFromT<T, IUnknown>&& DerivedFromT<U, IUnknown>
{
    return SharedPtr<T>(reinterpret_cast<T*>(other.Get()));
}

template <NonRefT T, NonRefT U>
constexpr SharedPtr<T> ReinterpretCast(SharedPtr<U>&& other) noexcept
    requires DerivedFromT<T, IUnknown>&& DerivedFromT<U, IUnknown>
{
    return SharedPtr<T>(reinterpret_cast<T*>(other.Detach()), false);
}

} // namespace mini