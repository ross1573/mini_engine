export module mini.core:atomic;

import :type;
import :memory;
import :atomic_base;
import :atomic_wait;
import :thread_base;

namespace mini::memory {

template <TrivialT T>
consteval SizeT AtomicAlignment()
{
    // current platform support atomic operations up to 16 bytes
    constexpr SizeT maxAlign = 1 << 4;
    if (sizeof(T) > maxAlign) return alignof(T);

    SizeT align = maxAlign;
    while (align > 0) {
        if ((align >> 1) < sizeof(T)) return align;
        align >>= 1;
    }

    return 1;
}

export using MemoryOrder = mini::memory::MemoryOrder;

export template <TrivialT T>
struct Atomic {
public:
    typedef T Value;

private:
    static_assert(sizeof(Value) != 0, "cannot create atomic value from incomplete type.");

    static constexpr SizeT alignment = AtomicAlignment<Value>();
    static constexpr SizeT pauseAttempt = 2;
    static constexpr SizeT yieldAttempt = 4;

    alignas(alignment) mutable Value m_Value;

public:
    constexpr Atomic() noexcept(NoThrowDefaultConstructibleT<T>) = default;
    constexpr Atomic(Value) noexcept;
    constexpr ~Atomic() noexcept = default;

    constexpr void Store(Value, MemoryOrder) noexcept;
    constexpr void Store(Value, MemoryOrder) volatile noexcept;
    constexpr Value Load(MemoryOrder) const noexcept;
    constexpr Value Load(MemoryOrder) const volatile noexcept;
    constexpr Value Exchange(Value, MemoryOrder) noexcept;
    constexpr Value Exchange(Value, MemoryOrder) volatile noexcept;

    constexpr bool CompareExchangeStrong(Value&, Value, MemoryOrder, MemoryOrder) noexcept;
    constexpr bool CompareExchangeStrong(Value&, Value, MemoryOrder, MemoryOrder) volatile noexcept;
    constexpr bool CompareExchangeStrong(Value&, Value, MemoryOrder) noexcept;
    constexpr bool CompareExchangeWeak(Value&, Value, MemoryOrder, MemoryOrder) noexcept;
    constexpr bool CompareExchangeWeak(Value&, Value, MemoryOrder, MemoryOrder) volatile noexcept;
    constexpr bool CompareExchangeWeak(Value&, Value, MemoryOrder) noexcept;
    constexpr bool CompareExchangeWeak(Value&, Value, MemoryOrder) volatile noexcept;

    constexpr Value FetchAdd(Value, MemoryOrder) noexcept
        requires IntegralT<Value>;
    constexpr Value FetchAdd(Value, MemoryOrder) volatile noexcept
        requires IntegralT<Value>;
    constexpr Value FetchSub(Value, MemoryOrder) noexcept
        requires IntegralT<Value>;
    constexpr Value FetchSub(Value, MemoryOrder) volatile noexcept
        requires IntegralT<Value>;
    constexpr Value FetchAnd(Value, MemoryOrder) noexcept
        requires IntegralT<Value>;
    constexpr Value FetchAnd(Value, MemoryOrder) volatile noexcept
        requires IntegralT<Value>;
    constexpr Value FetchXor(Value, MemoryOrder) noexcept
        requires IntegralT<Value>;
    constexpr Value FetchXor(Value, MemoryOrder) volatile noexcept
        requires IntegralT<Value>;
    constexpr Value FetchOr(Value, MemoryOrder) noexcept
        requires IntegralT<Value>;
    constexpr Value FetchOr(Value, MemoryOrder) volatile noexcept
        requires IntegralT<Value>;

    void Wait(Value, MemoryOrder) const noexcept;
    void Wait(Value, MemoryOrder) const volatile noexcept;
    void Notify() const noexcept;
    void Notify() const volatile noexcept;
    void NotifyAll() const noexcept;
    void NotifyAll() const volatile noexcept;

    static constexpr void ThreadFence(MemoryOrder order) noexcept;
    static constexpr bool IsLockFree() noexcept;
    static constexpr bool IsAlwaysLockFree() noexcept;

    constexpr operator Value() const noexcept;
    constexpr operator Value() const volatile noexcept;
    constexpr Value operator=(Value) noexcept;
    constexpr Value operator=(Value) volatile noexcept;

private:
    Atomic(Atomic const&) = delete;
    Atomic& operator=(Atomic const&) = delete;

    constexpr bool ConstexprCompareExchange(Value&, Value) volatile noexcept;

    constexpr void AssertStoreMemoryOrder(MemoryOrder) const noexcept;
    constexpr void AssertLoadMemoryOrder(MemoryOrder) const noexcept;
};

template <TrivialT T>
inline constexpr Atomic<T>::Atomic(Value val) noexcept
    : m_Value(val)
{
}

template <TrivialT T>
inline constexpr void Atomic<T>::Store(Value val, MemoryOrder order) noexcept
{
    AssertStoreMemoryOrder(order);

    if consteval {
        m_Value = val;
    }
    else {
        AtomicStore(&m_Value, &val, order);
    }
}

template <TrivialT T>
inline constexpr void Atomic<T>::Store(Value val, MemoryOrder order) volatile noexcept
{
    AssertStoreMemoryOrder(order);

    if consteval {
        m_Value = val;
    }
    else {
        AtomicStore(&m_Value, &val, order);
    }
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::Load(MemoryOrder order) const noexcept
{
    AssertLoadMemoryOrder(order);

    if consteval {
        return m_Value;
    }
    else {
        return AtomicLoad(&m_Value, order);
    }
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::Load(MemoryOrder order) const volatile noexcept
{
    AssertLoadMemoryOrder(order);

    if consteval {
        return m_Value;
    }
    else {
        return AtomicLoad(&m_Value, order);
    }
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::Exchange(Value val, MemoryOrder order) noexcept
{
    if consteval {
        m_Value = val;
        return m_Value;
    }
    else {
        return AtomicExchange(&m_Value, val, order);
    }
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::Exchange(Value val,
                                                      MemoryOrder order) volatile noexcept
{
    if consteval {
        m_Value = val;
        return m_Value;
    }
    else {
        return AtomicExchange(&m_Value, val, order);
    }
}

template <TrivialT T>
inline constexpr bool Atomic<T>::CompareExchangeStrong(Value& expected, Value desired,
                                                       MemoryOrder success,
                                                       MemoryOrder failure) noexcept
{
    if consteval {
        return ConstexprCompareExchange(expected, desired);
    }
    else {
        return AtomicCompareExchangeStrong(&m_Value, expected, desired, success, failure);
    }
}

template <TrivialT T>
inline constexpr bool Atomic<T>::CompareExchangeStrong(Value& expected, Value desired,
                                                       MemoryOrder success,
                                                       MemoryOrder failure) volatile noexcept
{
    if consteval {
        return ConstexprCompareExchange(expected, desired);
    }
    else {
        return AtomicCompareExchangeStrong(&m_Value, expected, desired, success, failure);
    }
}

template <TrivialT T>
inline constexpr bool Atomic<T>::CompareExchangeStrong(Value& expected, Value desired,
                                                       MemoryOrder order) noexcept
{
    if consteval {
        return ConstexprCompareExchange(expected, desired);
    }
    else {
        return AtomicCompareExchangeStrong(&m_Value, expected, desired, order, order);
    }
}

template <TrivialT T>
inline constexpr bool Atomic<T>::CompareExchangeWeak(Value& expected, Value desired,
                                                     MemoryOrder success,
                                                     MemoryOrder failure) noexcept
{
    if consteval {
        return ConstexprCompareExchange(expected, desired);
    }
    else {
        return AtomicCompareExchangeWeak(&m_Value, expected, desired, success, failure);
    }
}

template <TrivialT T>
inline constexpr bool Atomic<T>::CompareExchangeWeak(Value& expected, Value desired,
                                                     MemoryOrder success,
                                                     MemoryOrder failure) volatile noexcept
{
    if consteval {
        return ConstexprCompareExchange(expected, desired);
    }
    else {
        return AtomicCompareExchangeWeak(&m_Value, expected, desired, success, failure);
    }
}

template <TrivialT T>
inline constexpr bool Atomic<T>::CompareExchangeWeak(Value& expected, Value desired,
                                                     MemoryOrder order) noexcept
{
    if consteval {
        return ConstexprCompareExchange(expected, desired);
    }
    else {
        return AtomicCompareExchangeWeak(&m_Value, expected, desired, order, order);
    }
}

template <TrivialT T>
inline constexpr bool Atomic<T>::CompareExchangeWeak(Value& expected, Value desired,
                                                     MemoryOrder order) volatile noexcept
{
    if consteval {
        return ConstexprCompareExchange(expected, desired);
    }
    else {
        return AtomicCompareExchangeWeak(&m_Value, expected, desired, order, order);
    }
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchAdd(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value;
        m_Value += val;
        return tmp;
    }
    else {
        return AtomicFetchAdd(&m_Value, val, order);
    }
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchAdd(Value val,
                                                      MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value;
        m_Value += val;
        return tmp;
    }
    else {
        return AtomicFetchAdd(&m_Value, val, order);
    }
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchSub(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value;
        m_Value -= val;
        return tmp;
    }
    else {
        return AtomicFetchSub(&m_Value, val, order);
    }
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchSub(Value val,
                                                      MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value;
        m_Value -= val;
        return tmp;
    }
    else {
        return AtomicFetchSub(&m_Value, val, order);
    }
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchAnd(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value;
        m_Value &= val;
        return tmp;
    }
    else {
        return AtomicFetchAnd(&m_Value, val, order);
    }
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchAnd(Value val,
                                                      MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value;
        m_Value &= val;
        return tmp;
    }
    else {
        return AtomicFetchAnd(&m_Value, val, order);
    }
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchXor(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value;
        m_Value ^= val;
        return tmp;
    }
    else {
        return AtomicFetchXor(&m_Value, val, order);
    }
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchXor(Value val,
                                                      MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value;
        m_Value ^= val;
        return tmp;
    }
    else {
        return AtomicFetchXor(&m_Value, val, order);
    }
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchOr(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value;
        m_Value |= val;
        return tmp;
    }
    else {
        return AtomicFetchOr(&m_Value, val, order);
    }
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchOr(Value val, MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value;
        m_Value = !(m_Value & val);
        return tmp;
    }
    else {
        return AtomicFetchOr(&m_Value, val, order);
    }
}

template <TrivialT T>
inline void Atomic<T>::Wait(Value old, MemoryOrder order) const noexcept
{
    AssertLoadMemoryOrder(order);
    AtomicWait(&m_Value, old, order);
}

template <TrivialT T>
inline void Atomic<T>::Wait(Value old, MemoryOrder order) const volatile noexcept
{
    AssertLoadMemoryOrder(order);
    AtomicWait(&m_Value, old, order);
}

template <TrivialT T>
inline void Atomic<T>::Notify() const noexcept
{
    AtomicNotify(&m_Value);
}

template <TrivialT T>
inline void Atomic<T>::Notify() const volatile noexcept
{
    AtomicNotify(&m_Value);
}

template <TrivialT T>
inline void Atomic<T>::NotifyAll() const noexcept
{
    AtomicNotifyAll(&m_Value);
}

template <TrivialT T>
inline void Atomic<T>::NotifyAll() const volatile noexcept
{
    AtomicNotifyAll(&m_Value);
}

template <TrivialT T>
inline constexpr void Atomic<T>::ThreadFence(MemoryOrder order) noexcept
{
    if !consteval {
        AtomicThreadFence(order);
    }
}

template <TrivialT T>
inline constexpr bool Atomic<T>::IsLockFree() noexcept
{
    return AtomicIsLockFree(sizeof(Atomic<T>));
}

template <TrivialT T>
inline constexpr bool Atomic<T>::IsAlwaysLockFree() noexcept
{
    return AtomicAlwaysLockFree(sizeof(Atomic<T>));
}

template <TrivialT T>
inline constexpr Atomic<T>::operator Value() const noexcept
{
    return Load(MemoryOrder::sequential);
}

template <TrivialT T>
inline constexpr Atomic<T>::operator Value() const volatile noexcept
{
    return Load(MemoryOrder::sequential);
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::operator=(Value val) noexcept
{
    return Store(val, MemoryOrder::sequential);
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::operator=(Value val) volatile noexcept
{
    return Store(val, MemoryOrder::sequential);
}

template <TrivialT T>
inline constexpr bool Atomic<T>::ConstexprCompareExchange(Value& expected,
                                                          Value desired) volatile noexcept
{
    if (m_Value == expected) {
        m_Value = desired;
        return true;
    }

    expected = m_Value;
    return false;
}

template <TrivialT T>
inline constexpr void
Atomic<T>::AssertStoreMemoryOrder([[maybe_unused]] MemoryOrder order) const noexcept
{
    ASSERT(order == MemoryOrder::relaxed || order == MemoryOrder::sequential ||
               order == MemoryOrder::release,
           "invalid memory order on atomic store operation");
}

template <TrivialT T>
inline constexpr void
Atomic<T>::AssertLoadMemoryOrder([[maybe_unused]] MemoryOrder order) const noexcept
{
    ASSERT(order == MemoryOrder::relaxed || order == MemoryOrder::sequential ||
               order == MemoryOrder::acquire || order == MemoryOrder::consume,
           "invalid memory order on atomic load operation");
}

} // namespace mini::memory