export module mini.core:weak_ptr;

import :utility_operation;
import :shared_counter;
import :shared_ptr;

namespace mini {

export template <NonReferenceT T>
class WeakPtr {
private:
    template <NonReferenceT U>
    friend class SharedPtr;
    template <NonReferenceT U>
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
    constexpr WeakPtr(WeakPtr const& other) noexcept;
    constexpr WeakPtr(WeakPtr&& other) noexcept;

    template <NonReferenceT U>
    constexpr WeakPtr(WeakPtr<U> const& other) noexcept
        requires ConvertibleToT<U*, T*>;
    template <NonReferenceT U>
    constexpr WeakPtr(WeakPtr<U>&& other) noexcept
        requires ConvertibleToT<U*, T*>;
    template <NonReferenceT U>
    constexpr WeakPtr(SharedPtr<U> const& other) noexcept
        requires ConvertibleToT<U*, T*>;

    [[nodiscard]] constexpr SharedPtr<T> lock() const noexcept;
    [[nodiscard]] constexpr bool valid() const noexcept;

    constexpr void reset() noexcept;
    constexpr void swap(WeakPtr& other) noexcept;

    template <NonReferenceT U>
    [[nodiscard]] constexpr bool ownerEquals(WeakPtr<U> const& other) const noexcept
        requires EqualityComparableWithT<T*, U*>;
    template <NonReferenceT U>
    [[nodiscard]] constexpr bool ownerEquals(SharedPtr<U> const& other) const noexcept
        requires EqualityComparableWithT<T*, U*>;

    constexpr WeakPtr& operator=(WeakPtr const& other) noexcept;
    constexpr WeakPtr& operator=(WeakPtr&& other) noexcept;

    template <NonReferenceT U>
    constexpr WeakPtr& operator=(WeakPtr<U> const& other) noexcept
        requires ConvertibleToT<U*, T*>;
    template <NonReferenceT U>
    constexpr WeakPtr& operator=(WeakPtr<U>&& other) noexcept
        requires ConvertibleToT<U*, T*>;
    template <NonReferenceT U>
    constexpr WeakPtr& operator=(SharedPtr<U> const& other) noexcept
        requires ConvertibleToT<U*, T*>;
};

template <NonReferenceT T>
constexpr WeakPtr<T>::WeakPtr() noexcept
    : m_ptr(nullptr)
    , m_counter(nullptr)
{
}

template <NonReferenceT T>
constexpr WeakPtr<T>::~WeakPtr() noexcept
{
    if (m_counter != nullptr) {
        m_counter->releaseWeak();
        m_counter = nullptr;
        m_ptr = nullptr;
    }
}

template <NonReferenceT T>
constexpr WeakPtr<T>::WeakPtr(WeakPtr const& other) noexcept
    : m_ptr(other.m_ptr)
    , m_counter(other.m_counter)
{
    if (m_counter != nullptr) {
        m_counter->retainWeak();
    }
}

template <NonReferenceT T>
constexpr WeakPtr<T>::WeakPtr(WeakPtr&& other) noexcept
    : m_ptr(other.m_ptr)
    , m_counter(other.m_counter)
{
    other.m_ptr = nullptr;
    other.m_counter = nullptr;
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr WeakPtr<T>::WeakPtr(WeakPtr<U> const& other) noexcept
    requires ConvertibleToT<U*, T*>
    : m_ptr(static_cast<T*>(other.m_ptr))
    , m_counter(other.m_counter)
{
    if (m_counter != nullptr) {
        m_counter->retainWeak();
    }
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr WeakPtr<T>::WeakPtr(WeakPtr<U>&& other) noexcept
    requires ConvertibleToT<U*, T*>
    : m_ptr(static_cast<T*>(other.m_ptr))
    , m_counter(other.m_counter)
{
    other.m_ptr = nullptr;
    other.m_counter = nullptr;
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr WeakPtr<T>::WeakPtr(SharedPtr<U> const& other) noexcept
    requires ConvertibleToT<U*, T*>
    : m_ptr(static_cast<T*>(other.m_ptr))
    , m_counter(other.m_counter)
{
    if (m_counter != nullptr) {
        m_counter->retainWeak();
    }
}

template <NonReferenceT T>
constexpr SharedPtr<T> WeakPtr<T>::lock() const noexcept
{
    SharedPtr<T> result;
    result.m_counter = m_counter != nullptr ? m_counter->lock() : nullptr;
    if (result.m_counter) {
        result.m_ptr = m_ptr;
    }

    return result;
}

template <NonReferenceT T>
constexpr bool WeakPtr<T>::valid() const noexcept
{
    return m_counter != nullptr && m_counter->count() != 0;
}

template <NonReferenceT T>
constexpr void WeakPtr<T>::reset() noexcept
{
    if (m_counter != nullptr) {
        m_counter->releaseWeak();
        m_counter = nullptr;
        m_ptr = nullptr;
    }
}

template <NonReferenceT T>
constexpr void WeakPtr<T>::swap(WeakPtr& other) noexcept
{
    mini::swap(m_ptr, other.m_ptr);
    mini::swap(m_counter, other.m_counter);
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr bool WeakPtr<T>::ownerEquals(WeakPtr<U> const& other) const noexcept
    requires EqualityComparableWithT<T*, U*>
{
    return m_counter == other.m_counter;
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr bool WeakPtr<T>::ownerEquals(SharedPtr<U> const& other) const noexcept
    requires EqualityComparableWithT<T*, U*>
{
    return m_counter == other.m_counter;
}

template <NonReferenceT T>
constexpr WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr const& other) noexcept
{
    if (this == addressof(other)) {
        return *this;
    }

    if (m_counter != nullptr) {
        m_counter->releaseWeak();
    }

    m_ptr = other.m_ptr;
    m_counter = other.m_counter;

    if (m_counter != nullptr) {
        m_counter->retainWeak();
    }

    return *this;
}

template <NonReferenceT T>
constexpr WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr&& other) noexcept
{
    if (m_counter != nullptr) {
        m_counter->releaseWeak();
    }

    m_ptr = other.m_ptr;
    m_counter = other.m_counter;
    other.m_ptr = nullptr;
    other.m_counter = nullptr;

    return *this;
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr<U> const& other) noexcept
    requires ConvertibleToT<U*, T*>
{
    if (m_counter != nullptr) {
        m_counter->releaseWeak();
    }

    m_ptr = static_cast<T*>(other.m_ptr);
    m_counter = other.m_counter;

    if (m_counter != nullptr) {
        m_counter->retainWeak();
    }

    return *this;
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr<U>&& other) noexcept
    requires ConvertibleToT<U*, T*>
{
    if (m_counter != nullptr) {
        m_counter->releaseWeak();
    }

    m_ptr = static_cast<T*>(other.m_ptr);
    m_counter = other.m_counter;
    other.m_ptr = nullptr;
    other.m_counter = nullptr;

    return *this;
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr WeakPtr<T>& WeakPtr<T>::operator=(SharedPtr<U> const& other) noexcept
    requires ConvertibleToT<U*, T*>
{
    if (m_counter != nullptr) {
        m_counter->releaseWeak();
    }

    m_ptr = static_cast<T*>(other.m_ptr);
    m_counter = other.m_counter;

    if (m_counter != nullptr) {
        m_counter->retainWeak();
    }

    return *this;
}

} // namespace mini