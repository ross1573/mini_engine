export module mini.core:ptr_view;

import :type;
import :assert;
import :memory_operation;

namespace mini {

export template <NonReferenceT T>
class PtrView;

export template <typename T, typename U>
concept PointerLikeT = ConvertibleToT<T, PtrView<U>>;

template <NonReferenceT T>
class PtrView {
public:
    typedef T Value;
    typedef T* Pointer;
    typedef T& Reference;

private:
    Pointer m_ptr;

public:
    constexpr PtrView() noexcept;
    constexpr ~PtrView() noexcept = default;
    constexpr PtrView(PtrView const& other) noexcept = default;
    template <NonReferenceT U>
    constexpr PtrView(PtrView<U> other) noexcept
        requires ConvertibleToT<U*, T*>;
    template <NonReferenceT U>
    constexpr PtrView(U* ptr) noexcept
        requires ConvertibleToT<U*, T*>;

    constexpr void Reset() noexcept;
    template <NonReferenceT U>
    constexpr void Reset(U* ptr) noexcept
        requires ConvertibleToT<U*, T*>;

    [[nodiscard]] constexpr bool Valid() const noexcept;
    [[nodiscard]] constexpr Pointer Get() const noexcept;
    [[nodiscard]] constexpr Pointer* Address() noexcept;

    [[nodiscard]] constexpr Pointer* operator&() noexcept;
    [[nodiscard]] constexpr Pointer operator->() const noexcept;
    [[nodiscard]] constexpr Reference operator*() const noexcept;

    template <NonReferenceT U>
    constexpr operator U*() const noexcept
        requires ConvertibleToT<T*, U*>;

    constexpr PtrView& operator=(PtrView const& other) noexcept = default;
    constexpr PtrView& operator=(nullptr_t) noexcept;
    template <NonReferenceT U>
    constexpr PtrView& operator=(U* ptr) noexcept
        requires ConvertibleToT<U*, T*>;
    template <NonReferenceT U>
    constexpr PtrView& operator=(PtrView<U> other) noexcept
        requires ConvertibleToT<U*, T*>;
};

template <NonReferenceT T>
constexpr PtrView<T>::PtrView() noexcept
    : m_ptr(nullptr)
{
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr PtrView<T>::PtrView(PtrView<U> other) noexcept
    requires ConvertibleToT<U*, T*>
    : m_ptr(static_cast<Pointer>(other))
{
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr PtrView<T>::PtrView(U* ptr) noexcept
    requires ConvertibleToT<U*, T*>
    : m_ptr(static_cast<Pointer>(ptr))
{
}

template <NonReferenceT T>
constexpr void PtrView<T>::Reset() noexcept
{
    m_ptr = nullptr;
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr void PtrView<T>::Reset(U* ptr) noexcept
    requires ConvertibleToT<U*, T*>
{
    m_ptr = static_cast<Pointer>(ptr);
}

template <NonReferenceT T>
constexpr bool PtrView<T>::Valid() const noexcept
{
    if (m_ptr == nullptr) {
        return false;
    }

    if constexpr (ValidatableT<T>) {
        return m_ptr->Valid();
    }

    return true;
}

template <NonReferenceT T>
constexpr PtrView<T>::Pointer PtrView<T>::Get() const noexcept
{
    return m_ptr;
}

template <NonReferenceT T>
constexpr PtrView<T>::Pointer* PtrView<T>::Address() noexcept
{
    return memory::AddressOf(m_ptr);
}

template <NonReferenceT T>
constexpr PtrView<T>::Pointer* PtrView<T>::operator&() noexcept
{
    return memory::AddressOf(m_ptr);
}

template <NonReferenceT T>
constexpr PtrView<T>::Pointer PtrView<T>::operator->() const noexcept
{
    ASSERT(Valid(), "deference on invalid object pointer");
    return m_ptr;
}

template <NonReferenceT T>
constexpr PtrView<T>::Reference PtrView<T>::operator*() const noexcept
{
    ASSERT(Valid(), "deference on invalid object pointer");
    return *m_ptr;
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr PtrView<T>::operator U*() const noexcept
    requires ConvertibleToT<T*, U*>
{
    return m_ptr;
}

template <NonReferenceT T>
constexpr PtrView<T>& PtrView<T>::operator=(nullptr_t) noexcept
{
    m_ptr = nullptr;
    return *this;
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr PtrView<T>& PtrView<T>::operator=(U* ptr) noexcept
    requires ConvertibleToT<U*, T*>
{
    m_ptr = static_cast<Pointer>(ptr);
    return *this;
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr PtrView<T>& PtrView<T>::operator=(PtrView<U> other) noexcept
    requires ConvertibleToT<U*, T*>
{
    m_ptr = static_cast<Pointer>(other.m_ptr);
    return *this;
}

export template <NonReferenceT T, NonReferenceT U>
constexpr bool operator==(PtrView<T> lhs, PtrView<U> rhs) noexcept
{
    return lhs.Get() == rhs.Get();
}

export template <NonReferenceT T, NonReferenceT U>
constexpr auto operator<=>(PtrView<T> lhs, PtrView<U> rhs) noexcept
{
    return lhs.Get() <=> rhs.Get();
}

export template <NonReferenceT T>
constexpr bool operator==(PtrView<T> ptr, nullptr_t) noexcept
{
    return ptr.Get() == nullptr;
}

export template <NonReferenceT T>
constexpr auto operator<=>(PtrView<T> ptr, nullptr_t) noexcept
{
    return ptr.Get() <=> nullptr;
}

export template <NonReferenceT T, NonReferenceT U>
constexpr PtrView<T> StaticCast(PtrView<U> other) noexcept
{
    return PtrView<T>(static_cast<PtrView<T>::Pointer>(other.Get()));
}

export template <NonReferenceT T, NonReferenceT U>
constexpr PtrView<T> DynamicCast(PtrView<U> other) noexcept
{
    return PtrView<T>(dynamic_cast<PtrView<T>::Pointer>(other.Get()));
}

export template <NonReferenceT T, NonReferenceT U>
constexpr PtrView<T> ConstCast(PtrView<U> other) noexcept
{
    return PtrView<T>(const_cast<PtrView<T>::Pointer>(other.Get()));
}

export template <NonReferenceT T, NonReferenceT U>
constexpr PtrView<T> ReinterpretCast(PtrView<U> other) noexcept
{
    return PtrView<T>(reinterpret_cast<PtrView<T>::Pointer>(other.Get()));
}

} // namespace mini