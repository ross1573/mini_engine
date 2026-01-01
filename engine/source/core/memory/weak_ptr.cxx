export module mini.core:weak_ptr;

import :utility_operation;
import :shared_counter;
import :shared_ptr;

namespace mini {

export template <NonRefT T>
class WeakPtr {
private:
    template <NonRefT U>
    friend class SharedPtr;
    template <NonRefT U>
    friend class WeakPtr;

public:
    typedef T Value;
    typedef T* Ptr;
    typedef T& Ref;

private:
    Ptr m_Ptr;
    SharedCounter* m_Counter;

public:
    constexpr WeakPtr() noexcept;
    constexpr ~WeakPtr() noexcept;
    constexpr WeakPtr(WeakPtr const&) noexcept;
    constexpr WeakPtr(WeakPtr&&) noexcept;

    template <PtrConvertibleToT<T> U>
    constexpr WeakPtr(WeakPtr<U> const&) noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr WeakPtr(WeakPtr<U>&&) noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr WeakPtr(SharedPtr<U> const&) noexcept;

    constexpr SharedPtr<T> Lock() const noexcept;
    constexpr bool IsValid() const noexcept;

    constexpr void Reset() noexcept;
    constexpr void Swap(WeakPtr&) noexcept;

    template <NonRefT U>
    constexpr bool OwnerEquals(WeakPtr<U> const&) const noexcept
        requires EqualityComparableWithT<T*, U*>;
    template <NonRefT U>
    constexpr bool OwnerEquals(SharedPtr<U> const&) const noexcept
        requires EqualityComparableWithT<T*, U*>;

    constexpr WeakPtr& operator=(WeakPtr const&) noexcept;
    constexpr WeakPtr& operator=(WeakPtr&&) noexcept;

    template <PtrConvertibleToT<T> U>
    constexpr WeakPtr& operator=(WeakPtr<U> const&) noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr WeakPtr& operator=(WeakPtr<U>&&) noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr WeakPtr& operator=(SharedPtr<U> const&) noexcept;
};

template <NonRefT T>
inline constexpr WeakPtr<T>::WeakPtr() noexcept
    : m_Ptr(nullptr)
    , m_Counter(nullptr)
{
}

template <NonRefT T>
inline constexpr WeakPtr<T>::~WeakPtr() noexcept
{
    if (m_Counter != nullptr) {
        m_Counter->ReleaseWeak();
        m_Counter = nullptr;
        m_Ptr = nullptr;
    }
}

template <NonRefT T>
inline constexpr WeakPtr<T>::WeakPtr(WeakPtr const& other) noexcept
    : m_Ptr(other.m_Ptr)
    , m_Counter(other.m_Counter)
{
    if (m_Counter != nullptr) {
        m_Counter->RetainWeak();
    }
}

template <NonRefT T>
inline constexpr WeakPtr<T>::WeakPtr(WeakPtr&& other) noexcept
    : m_Ptr(other.m_Ptr)
    , m_Counter(other.m_Counter)
{
    other.m_Ptr = nullptr;
    other.m_Counter = nullptr;
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
inline constexpr WeakPtr<T>::WeakPtr(WeakPtr<U> const& other) noexcept
    : m_Ptr(static_cast<T*>(other.m_Ptr))
    , m_Counter(other.m_Counter)
{
    if (m_Counter != nullptr) {
        m_Counter->RetainWeak();
    }
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
inline constexpr WeakPtr<T>::WeakPtr(WeakPtr<U>&& other) noexcept
    : m_Ptr(static_cast<T*>(other.m_Ptr))
    , m_Counter(other.m_Counter)
{
    other.m_Ptr = nullptr;
    other.m_Counter = nullptr;
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
inline constexpr WeakPtr<T>::WeakPtr(SharedPtr<U> const& other) noexcept
    : m_Ptr(static_cast<T*>(other.m_Ptr))
    , m_Counter(other.m_Counter)
{
    if (m_Counter != nullptr) {
        m_Counter->RetainWeak();
    }
}

template <NonRefT T>
constexpr SharedPtr<T> WeakPtr<T>::Lock() const noexcept
{
    SharedPtr<T> result;
    result.m_Counter = m_Counter != nullptr ? m_Counter->Lock() : nullptr;
    if (result.m_Counter) {
        result.m_Ptr = m_Ptr;
    }

    return result;
}

template <NonRefT T>
constexpr bool WeakPtr<T>::IsValid() const noexcept
{
    return m_Counter != nullptr && m_Counter->Count() != 0;
}

template <NonRefT T>
constexpr void WeakPtr<T>::Reset() noexcept
{
    if (m_Counter != nullptr) {
        m_Counter->ReleaseWeak();
        m_Counter = nullptr;
        m_Ptr = nullptr;
    }
}

template <NonRefT T>
constexpr void WeakPtr<T>::Swap(WeakPtr& other) noexcept
{
    mini::Swap(m_Ptr, other.m_Ptr);
    mini::Swap(m_Counter, other.m_Counter);
}

template <NonRefT T>
template <NonRefT U>
constexpr bool WeakPtr<T>::OwnerEquals(WeakPtr<U> const& other) const noexcept
    requires EqualityComparableWithT<T*, U*>
{
    return m_Counter == other.m_Counter;
}

template <NonRefT T>
template <NonRefT U>
constexpr bool WeakPtr<T>::OwnerEquals(SharedPtr<U> const& other) const noexcept
    requires EqualityComparableWithT<T*, U*>
{
    return m_Counter == other.m_Counter;
}

template <NonRefT T>
constexpr WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr const& other) noexcept
{
    if (m_Counter != nullptr) {
        m_Counter->ReleaseWeak();
    }

    m_Ptr = other.m_Ptr;
    m_Counter = other.m_Counter;

    if (m_Counter != nullptr) {
        m_Counter->RetainWeak();
    }

    return *this;
}

template <NonRefT T>
constexpr WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr&& other) noexcept
{
    if (m_Counter != nullptr) {
        m_Counter->ReleaseWeak();
    }

    m_Ptr = other.m_Ptr;
    m_Counter = other.m_Counter;
    other.m_Ptr = nullptr;
    other.m_Counter = nullptr;

    return *this;
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
constexpr WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr<U> const& other) noexcept
{
    if (m_Counter != nullptr) {
        m_Counter->ReleaseWeak();
    }

    m_Ptr = static_cast<T*>(other.m_Ptr);
    m_Counter = other.m_Counter;

    if (m_Counter != nullptr) {
        m_Counter->RetainWeak();
    }

    return *this;
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
constexpr WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr<U>&& other) noexcept
{
    if (m_Counter != nullptr) {
        m_Counter->ReleaseWeak();
    }

    m_Ptr = static_cast<T*>(other.m_Ptr);
    m_Counter = other.m_Counter;
    other.m_Ptr = nullptr;
    other.m_Counter = nullptr;

    return *this;
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
constexpr WeakPtr<T>& WeakPtr<T>::operator=(SharedPtr<U> const& other) noexcept
{
    if (m_Counter != nullptr) {
        m_Counter->ReleaseWeak();
    }

    m_Ptr = static_cast<T*>(other.m_Ptr);
    m_Counter = other.m_Counter;

    if (m_Counter != nullptr) {
        m_Counter->RetainWeak();
    }

    return *this;
}

} // namespace mini