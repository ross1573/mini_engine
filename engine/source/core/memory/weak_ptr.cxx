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
    typedef T* Pointer;
    typedef T& Reference;

private:
    Pointer m_ptr;
    SharedCounter* m_counter;

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
    constexpr bool Valid() const noexcept;

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
    : m_ptr(nullptr)
    , m_counter(nullptr)
{
}

template <NonRefT T>
inline constexpr WeakPtr<T>::~WeakPtr() noexcept
{
    if (m_counter != nullptr) {
        m_counter->ReleaseWeak();
        m_counter = nullptr;
        m_ptr = nullptr;
    }
}

template <NonRefT T>
inline constexpr WeakPtr<T>::WeakPtr(WeakPtr const& other) noexcept
    : m_ptr(other.m_ptr)
    , m_counter(other.m_counter)
{
    if (m_counter != nullptr) {
        m_counter->RetainWeak();
    }
}

template <NonRefT T>
inline constexpr WeakPtr<T>::WeakPtr(WeakPtr&& other) noexcept
    : m_ptr(other.m_ptr)
    , m_counter(other.m_counter)
{
    other.m_ptr = nullptr;
    other.m_counter = nullptr;
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
inline constexpr WeakPtr<T>::WeakPtr(WeakPtr<U> const& other) noexcept
    : m_ptr(static_cast<T*>(other.m_ptr))
    , m_counter(other.m_counter)
{
    if (m_counter != nullptr) {
        m_counter->RetainWeak();
    }
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
inline constexpr WeakPtr<T>::WeakPtr(WeakPtr<U>&& other) noexcept
    : m_ptr(static_cast<T*>(other.m_ptr))
    , m_counter(other.m_counter)
{
    other.m_ptr = nullptr;
    other.m_counter = nullptr;
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
inline constexpr WeakPtr<T>::WeakPtr(SharedPtr<U> const& other) noexcept
    : m_ptr(static_cast<T*>(other.m_ptr))
    , m_counter(other.m_counter)
{
    if (m_counter != nullptr) {
        m_counter->RetainWeak();
    }
}

template <NonRefT T>
constexpr SharedPtr<T> WeakPtr<T>::Lock() const noexcept
{
    SharedPtr<T> result;
    result.m_counter = m_counter != nullptr ? m_counter->Lock() : nullptr;
    if (result.m_counter) {
        result.m_ptr = m_ptr;
    }

    return result;
}

template <NonRefT T>
constexpr bool WeakPtr<T>::Valid() const noexcept
{
    return m_counter != nullptr && m_counter->Count() != 0;
}

template <NonRefT T>
constexpr void WeakPtr<T>::Reset() noexcept
{
    if (m_counter != nullptr) {
        m_counter->ReleaseWeak();
        m_counter = nullptr;
        m_ptr = nullptr;
    }
}

template <NonRefT T>
constexpr void WeakPtr<T>::Swap(WeakPtr& other) noexcept
{
    mini::Swap(m_ptr, other.m_ptr);
    mini::Swap(m_counter, other.m_counter);
}

template <NonRefT T>
template <NonRefT U>
constexpr bool WeakPtr<T>::OwnerEquals(WeakPtr<U> const& other) const noexcept
    requires EqualityComparableWithT<T*, U*>
{
    return m_counter == other.m_counter;
}

template <NonRefT T>
template <NonRefT U>
constexpr bool WeakPtr<T>::OwnerEquals(SharedPtr<U> const& other) const noexcept
    requires EqualityComparableWithT<T*, U*>
{
    return m_counter == other.m_counter;
}

template <NonRefT T>
constexpr WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr const& other) noexcept
{
    if (m_counter != nullptr) {
        m_counter->ReleaseWeak();
    }

    m_ptr = other.m_ptr;
    m_counter = other.m_counter;

    if (m_counter != nullptr) {
        m_counter->RetainWeak();
    }

    return *this;
}

template <NonRefT T>
constexpr WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr&& other) noexcept
{
    if (m_counter != nullptr) {
        m_counter->ReleaseWeak();
    }

    m_ptr = other.m_ptr;
    m_counter = other.m_counter;
    other.m_ptr = nullptr;
    other.m_counter = nullptr;

    return *this;
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
constexpr WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr<U> const& other) noexcept
{
    if (m_counter != nullptr) {
        m_counter->ReleaseWeak();
    }

    m_ptr = static_cast<T*>(other.m_ptr);
    m_counter = other.m_counter;

    if (m_counter != nullptr) {
        m_counter->RetainWeak();
    }

    return *this;
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
constexpr WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr<U>&& other) noexcept
{
    if (m_counter != nullptr) {
        m_counter->ReleaseWeak();
    }

    m_ptr = static_cast<T*>(other.m_ptr);
    m_counter = other.m_counter;
    other.m_ptr = nullptr;
    other.m_counter = nullptr;

    return *this;
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
constexpr WeakPtr<T>& WeakPtr<T>::operator=(SharedPtr<U> const& other) noexcept
{
    if (m_counter != nullptr) {
        m_counter->ReleaseWeak();
    }

    m_ptr = static_cast<T*>(other.m_ptr);
    m_counter = other.m_counter;

    if (m_counter != nullptr) {
        m_counter->RetainWeak();
    }

    return *this;
}

} // namespace mini