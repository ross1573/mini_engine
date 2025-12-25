module;

#if CLANG // TODO: clang some how manages to skip this diagnostic
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Watomic-alignment"
#endif

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
import :numeric;
import :memory_operation;
import :atomic_base;
import :atomic_platform;
import :atomic_wait;

namespace mini {

export using MemoryOrder = MemoryOrder;

export template <TrivialT T>
struct Atomic {
private:
    typedef AtomicBase<T> Base;

public:
    typedef typename Base::Value Value;

private:
    mutable Base m_Value;

public:
    constexpr Atomic() noexcept(NoThrowDefaultConstructibleT<T>)
        requires DefaultConstructibleT<T>;
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
    Atomic& operator=(Atomic const&) volatile = delete;

    constexpr bool ConstexprCompareExchange(Value&, Value) noexcept;
    constexpr bool ConstexprCompareExchange(Value volatile&, Value) volatile noexcept;
};

template <TrivialT T>
constexpr Atomic<T>::Atomic() noexcept(NoThrowDefaultConstructibleT<T>)
    requires DefaultConstructibleT<T>
    : m_Value(T())
{
}

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
        m_Value.value = val;
        return;
    }

    __atomic_store(memory::AddressOf(m_Value.value), &val, static_cast<int>(order));
}

template <TrivialT T>
inline constexpr void Atomic<T>::Store(Value val, MemoryOrder order) volatile noexcept
    [[diagnose_store(order)]]
{
    if consteval {
        m_Value.value = val;
        return;
    }

    __atomic_store(memory::AddressOf(m_Value.value), &val, static_cast<int>(order));
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::Load(MemoryOrder order) const noexcept
    [[diagnose_load(order)]]
{
    if consteval {
        return m_Value.value;
    }

    Value result;
    __atomic_load(memory::AddressOf(m_Value.value), &result, static_cast<int>(order));
    return result;
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::Load(MemoryOrder order) const volatile noexcept
    [[diagnose_load(order)]]
{
    if consteval {
        return m_Value.value;
    }

    Value result;
    __atomic_load(memory::AddressOf(m_Value.value), &result, static_cast<int>(order));
    return result;
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::Exchange(Value val, MemoryOrder order) noexcept
{
    if consteval {
        m_Value.value = val;
        return m_Value.value;
    }

    Value result;
    __atomic_exchange(memory::AddressOf(m_Value.value), &val, &result, static_cast<int>(order));
    return result;
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::Exchange(Value val,
                                                      MemoryOrder order) volatile noexcept
{
    if consteval {
        m_Value.value = val;
        return m_Value.value;
    }

    Value result;
    __atomic_exchange(memory::AddressOf(m_Value.value), &val, &result, static_cast<int>(order));
    return result;
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

    return __atomic_compare_exchange(memory::AddressOf(m_Value.value), &expected, &desired, false,
                                     static_cast<int>(success), static_cast<int>(failure));
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

    return __atomic_compare_exchange(memory::AddressOf(m_Value.value), &expected, &desired, false,
                                     static_cast<int>(success), static_cast<int>(failure));
}

template <TrivialT T>
inline constexpr bool Atomic<T>::CompareExchangeStrong(Value& expected, Value desired,
                                                       MemoryOrder order) noexcept
{
    if consteval {
        return ConstexprCompareExchange(expected, desired);
    }

    return __atomic_compare_exchange(memory::AddressOf(m_Value.value), &expected, &desired, false,
                                     static_cast<int>(order), FailureOrder(order));
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

    return __atomic_compare_exchange(memory::AddressOf(m_Value.value), &expected, &desired, true,
                                     static_cast<int>(success), static_cast<int>(failure));
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

    return __atomic_compare_exchange(memory::AddressOf(m_Value.value), &expected, &desired, true,
                                     static_cast<int>(success), static_cast<int>(failure));
}

template <TrivialT T>
inline constexpr bool Atomic<T>::CompareExchangeWeak(Value& expected, Value desired,
                                                     MemoryOrder order) noexcept
{
    if consteval {
        return ConstexprCompareExchange(expected, desired);
    }

    return __atomic_compare_exchange(memory::AddressOf(m_Value.value), &expected, &desired, true,
                                     static_cast<int>(order), FailureOrder(order));
}

template <TrivialT T>
inline constexpr bool Atomic<T>::CompareExchangeWeak(Value& expected, Value desired,
                                                     MemoryOrder order) volatile noexcept
{
    if consteval {
        return ConstexprCompareExchange(expected, desired);
    }

    return __atomic_compare_exchange(memory::AddressOf(m_Value.value), &expected, &desired, true,
                                     static_cast<int>(order), FailureOrder(order));
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchAdd(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value.value;
        m_Value.value += val;
        return tmp;
    }

    return __atomic_fetch_add(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchAdd(Value val,
                                                      MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value.value;
        m_Value.value += val;
        return tmp;
    }

    return __atomic_fetch_add(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchSub(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value.value;
        m_Value.value -= val;
        return tmp;
    }

    return __atomic_fetch_sub(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchSub(Value val,
                                                      MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value.value;
        m_Value.value -= val;
        return tmp;
    }

    return __atomic_fetch_sub(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchAnd(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value.value;
        m_Value.value &= val;
        return tmp;
    }

    return __atomic_fetch_and(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchAnd(Value val,
                                                      MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value.value;
        m_Value.value &= val;
        return tmp;
    }

    return __atomic_fetch_and(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchXor(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value.value;
        m_Value.value ^= val;
        return tmp;
    }

    return __atomic_fetch_xor(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchXor(Value val,
                                                      MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value.value;
        m_Value.value ^= val;
        return tmp;
    }

    return __atomic_fetch_xor(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchOr(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value.value;
        m_Value.value |= val;
        return tmp;
    }

    return __atomic_fetch_or(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline constexpr Atomic<T>::Value Atomic<T>::FetchOr(Value val, MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    if consteval {
        T tmp = m_Value.value;
        m_Value.value = !(m_Value & val);
        return tmp;
    }

    return __atomic_fetch_or(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline void Atomic<T>::Wait(Value old, MemoryOrder order) const noexcept [[diagnose_wait(order)]]
{
    __atomic_wait(memory::AddressOf(m_Value.value), old, static_cast<int32>(order));
}

template <TrivialT T>
inline void Atomic<T>::Wait(Value old, MemoryOrder order) const volatile noexcept
    [[diagnose_wait(order)]]
{
    __atomic_wait(memory::AddressOf(m_Value.value), old, static_cast<int32>(order));
}

template <TrivialT T>
inline void Atomic<T>::Notify() const noexcept
{
    __atomic_notify_one(memory::AddressOf(m_Value.value));
}

template <TrivialT T>
inline void Atomic<T>::Notify() const volatile noexcept
{
    __atomic_notify_one(memory::AddressOf(m_Value.value));
}

template <TrivialT T>
inline void Atomic<T>::NotifyAll() const noexcept
{
    __atomic_notify_all(memory::AddressOf(m_Value.value));
}

template <TrivialT T>
inline void Atomic<T>::NotifyAll() const volatile noexcept
{
    __atomic_notify_all(memory::AddressOf(m_Value.value));
}

template <TrivialT T>
inline bool Atomic<T>::IsLockFree() const noexcept
{
    void const* loc = static_cast<void const*>(memory::AddressOf(m_Value.value));
    return __atomic_is_lock_free(sizeof(Base), const_cast<void*>(loc));
}

template <TrivialT T>
inline bool Atomic<T>::IsLockFree() const volatile noexcept
{
    void const volatile* loc = static_cast<void const volatile*>(memory::AddressOf(m_Value.value));
    return __atomic_is_lock_free(sizeof(Base), const_cast<void*>(loc));
}

template <TrivialT T>
inline constexpr bool Atomic<T>::IsAlwaysLockFree() noexcept
{
    return __atomic_always_lock_free(sizeof(Base), nullptr);
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
        __atomic_thread_fence(static_cast<int>(order));
    }
}

template <TrivialT T>
inline constexpr void Atomic<T>::SignalFence(MemoryOrder order) noexcept
{
    if !consteval {
        __atomic_signal_fence(static_cast<int>(order));
    }
}

template <TrivialT T>
inline constexpr bool Atomic<T>::ConstexprCompareExchange(Value& expected, Value desired) noexcept
{
    if (m_Value.value == expected) {
        m_Value.value = desired;
        return true;
    }

    expected = m_Value.value;
    return false;
}

template <TrivialT T>
inline constexpr bool Atomic<T>::ConstexprCompareExchange(Value volatile& expected,
                                                          Value desired) volatile noexcept
{
    if (static_cast<Value volatile>(m_Value.value) == expected) {
        m_Value.value = static_cast<Value>(desired);
        return true;
    }

    expected = static_cast<Value volatile>(m_Value.value);
    return false;
}

} // namespace mini