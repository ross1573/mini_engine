module;

#define diagnose_store(order)                                                                \
    diagnose_error((order == MemoryOrder::acquire || order == MemoryOrder::acquireRelease || \
                    order == MemoryOrder::consume),                                          \
                   "invalid memory order on atomic store operation")

#define diagnose_load(order)                                                                \
    diagnose_error((order == MemoryOrder::release || order == MemoryOrder::acquireRelease), \
                   "invalid memory order on atomic load operation")

#define diagnose_compare_exchange(order)                                                    \
    diagnose_error((order == MemoryOrder::release || order == MemoryOrder::acquireRelease), \
                   "invalid memory order on atomic exchange operation")

#define diagnose_wait(order)                                                               \
    diagnose_wait((order == MemoryOrder::release || order == MemoryOrder::acquireRelease), \
                  "invalid memory order on atomic wait operation")

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

    bool IsLockFree() const noexcept;
    bool IsLockFree() const volatile noexcept;
    static constexpr bool IsAlwaysLockFree() noexcept;

    static constexpr void ThreadFence(MemoryOrder order) noexcept;
    static constexpr void SignalFence(MemoryOrder order) noexcept;

    constexpr operator Value() const noexcept;
    constexpr operator Value() const volatile noexcept;
    constexpr Value operator=(Value) noexcept;
    constexpr Value operator=(Value) volatile noexcept;

private:
    Atomic(Atomic const&) = delete;
    Atomic& operator=(Atomic const&) = delete;

    constexpr bool ConstexprCompareExchange(Value&, Value) noexcept;
    constexpr bool ConstexprCompareExchange(Value volatile&, Value) volatile noexcept;
};

template <TrivialT T>
inline constexpr Atomic<T>::Atomic(Value val) noexcept
    : m_Value(val)
{
}

template <TrivialT T>
inline constexpr void Atomic<T>::Store(Value val, MemoryOrder order) noexcept
    [[diagnose_store(order)]]
{
    if consteval {
        m_Value = val;
        return;
    }

    AtomicStore(&m_Value, val, order);
}

template <TrivialT T>
inline constexpr void Atomic<T>::Store(Value val, MemoryOrder order) volatile noexcept
    [[diagnose_store(order)]]
{
    if consteval {
        m_Value = val;
        return;
    }

    AtomicStore(&m_Value, val, order);
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::Load(MemoryOrder order) const noexcept
    [[diagnose_load(order)]]
{
    if consteval {
        return m_Value;
    }

    return AtomicLoad(&m_Value, order);
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::Load(MemoryOrder order) const volatile noexcept
    [[diagnose_load(order)]]
{
    if consteval {
        return m_Value;
    }

    return AtomicLoad(&m_Value, order);
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::Exchange(Value val, MemoryOrder order) noexcept
{
    if consteval {
        m_Value = val;
        return m_Value;
    }

    return AtomicExchange(&m_Value, val, order);
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::Exchange(Value val,
                                                      MemoryOrder order) volatile noexcept
{
    if consteval {
        m_Value = val;
        return m_Value;
    }

    return AtomicExchange(&m_Value, val, order);
}

template <TrivialT T>
inline constexpr bool Atomic<T>::CompareExchangeStrong(Value& expected, Value desired,
                                                       MemoryOrder success,
                                                       MemoryOrder failure) noexcept
    [[diagnose_compare_exchange(failure)]]
{
    if consteval {
        return ConstexprCompareExchange(expected, desired);
    }

    return AtomicCompareExchangeStrong(&m_Value, &expected, desired, success, failure);
}

template <TrivialT T>
inline constexpr bool Atomic<T>::CompareExchangeStrong(Value& expected, Value desired,
                                                       MemoryOrder success,
                                                       MemoryOrder failure) volatile noexcept
    [[diagnose_compare_exchange(failure)]]
{
    if consteval {
        return ConstexprCompareExchange(expected, desired);
    }

    return AtomicCompareExchangeStrong(&m_Value, &expected, desired, success, failure);
}

template <TrivialT T>
inline constexpr bool Atomic<T>::CompareExchangeStrong(Value& expected, Value desired,
                                                       MemoryOrder order) noexcept
{
    if consteval {
        return ConstexprCompareExchange(expected, desired);
    }

    return AtomicCompareExchangeStrong(&m_Value, &expected, desired, order, order);
}

template <TrivialT T>
inline constexpr bool Atomic<T>::CompareExchangeWeak(Value& expected, Value desired,
                                                     MemoryOrder success,
                                                     MemoryOrder failure) noexcept
    [[diagnose_compare_exchange(failure)]]
{
    if consteval {
        return ConstexprCompareExchange(expected, desired);
    }

    return AtomicCompareExchangeWeak(&m_Value, &expected, desired, success, failure);
}

template <TrivialT T>
inline constexpr bool Atomic<T>::CompareExchangeWeak(Value& expected, Value desired,
                                                     MemoryOrder success,
                                                     MemoryOrder failure) volatile noexcept
    [[diagnose_compare_exchange(failure)]]
{
    if consteval {
        return ConstexprCompareExchange(expected, desired);
    }

    return AtomicCompareExchangeWeak(&m_Value, &expected, desired, success, failure);
}

template <TrivialT T>
inline constexpr bool Atomic<T>::CompareExchangeWeak(Value& expected, Value desired,
                                                     MemoryOrder order) noexcept
{
    if consteval {
        return ConstexprCompareExchange(expected, desired);
    }

    return AtomicCompareExchangeWeak(&m_Value, &expected, desired, order, order);
}

template <TrivialT T>
inline constexpr bool Atomic<T>::CompareExchangeWeak(Value& expected, Value desired,
                                                     MemoryOrder order) volatile noexcept
{
    if consteval {
        return ConstexprCompareExchange(expected, desired);
    }

    return AtomicCompareExchangeWeak(&m_Value, &expected, desired, order, order);
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

    return AtomicFetchAdd(&m_Value, val, order);
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

    return AtomicFetchAdd(&m_Value, val, order);
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

    return AtomicFetchSub(&m_Value, val, order);
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

    return AtomicFetchSub(&m_Value, val, order);
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

    return AtomicFetchAnd(&m_Value, val, order);
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

    return AtomicFetchAnd(&m_Value, val, order);
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

    return AtomicFetchXor(&m_Value, val, order);
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

    return AtomicFetchXor(&m_Value, val, order);
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

    return AtomicFetchOr(&m_Value, val, order);
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

    return AtomicFetchOr(&m_Value, val, order);
}

template <TrivialT T>
inline void Atomic<T>::Wait(Value old, MemoryOrder order) const noexcept [[diagnose_wait(order)]]
{
    AtomicWait(&m_Value, old, order);
}

template <TrivialT T>
inline void Atomic<T>::Wait(Value old, MemoryOrder order) const volatile noexcept
    [[diagnose_wait(order)]]
{
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
inline bool Atomic<T>::IsLockFree() const noexcept
{
    return AtomicIsLockFree(sizeof(Atomic<T>), static_cast<void*>(&m_Value));
}

template <TrivialT T>
inline bool Atomic<T>::IsLockFree() const volatile noexcept
{
    return AtomicIsLockFree(sizeof(Atomic<T>), static_cast<void volatile*>(&m_Value));
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
inline constexpr void Atomic<T>::ThreadFence(MemoryOrder order) noexcept
{
    if !consteval {
        AtomicThreadFence(order);
    }
}

template <TrivialT T>
inline constexpr void Atomic<T>::SignalFence(MemoryOrder order) noexcept
{
    if !consteval {
        AtomicSignalFence(order);
    }
}

template <TrivialT T>
inline constexpr bool Atomic<T>::ConstexprCompareExchange(Value& expected, Value desired) noexcept
{
    if (m_Value == expected) {
        m_Value = desired;
        return true;
    }

    expected = m_Value;
    return false;
}

template <TrivialT T>
inline constexpr bool Atomic<T>::ConstexprCompareExchange(Value volatile& expected,
                                                          Value desired) volatile noexcept
{
    if (static_cast<Value volatile>(m_Value) == expected) {
        m_Value = static_cast<Value>(desired);
        return true;
    }

    expected = static_cast<Value volatile>(m_Value);
    return false;
}

} // namespace mini::memory