module;

#if CLANG // TODO: clang some how manages to skip this diagnostic
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Watomic-alignment"
#endif

#define diagnose_store(order)                                                 \
    diagnose_error((order == __ATOMIC_ACQUIRE || order == __ATOMIC_ACQ_REL || \
                    order == __ATOMIC_CONSUME),                               \
                   "invalid memory order on atomic store operation")

#define diagnose_load(order)                                                 \
    diagnose_error((order == __ATOMIC_RELEASE || order == __ATOMIC_ACQ_REL), \
                   "invalid memory order on atomic load operation")

#define diagnose_compare_exchange(order)                                     \
    diagnose_error((order == __ATOMIC_RELEASE || order == __ATOMIC_ACQ_REL), \
                   "invalid memory order on atomic exchange operation")

#define diagnose_wait(order)                                                \
    diagnose_wait((order == __ATOMIC_RELEASE || order == __ATOMIC_ACQ_REL), \
                  "invalid memory order on atomic wait operation")

export module mini.core:atomic;

import :type;
import :numeric;
import :memory_operation;
import :atomic_base;
import :atomic_platform;
import :atomic_wait;

namespace mini {

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

    void Store(Value, MemoryOrder) noexcept;
    void Store(Value, MemoryOrder) volatile noexcept;
    Value Load(MemoryOrder) const noexcept;
    Value Load(MemoryOrder) const volatile noexcept;
    Value Exchange(Value, MemoryOrder) noexcept;
    Value Exchange(Value, MemoryOrder) volatile noexcept;

    bool CompareExchangeStrong(Value&, Value, MemoryOrder, MemoryOrder) noexcept;
    bool CompareExchangeStrong(Value&, Value, MemoryOrder, MemoryOrder) volatile noexcept;
    bool CompareExchangeStrong(Value&, Value, MemoryOrder) noexcept;
    bool CompareExchangeWeak(Value&, Value, MemoryOrder, MemoryOrder) noexcept;
    bool CompareExchangeWeak(Value&, Value, MemoryOrder, MemoryOrder) volatile noexcept;
    bool CompareExchangeWeak(Value&, Value, MemoryOrder) noexcept;
    bool CompareExchangeWeak(Value&, Value, MemoryOrder) volatile noexcept;

    Value FetchAdd(Value, MemoryOrder) noexcept
        requires IntegralT<Value>;
    Value FetchAdd(Value, MemoryOrder) volatile noexcept
        requires IntegralT<Value>;
    Value FetchSub(Value, MemoryOrder) noexcept
        requires IntegralT<Value>;
    Value FetchSub(Value, MemoryOrder) volatile noexcept
        requires IntegralT<Value>;
    Value FetchAnd(Value, MemoryOrder) noexcept
        requires IntegralT<Value>;
    Value FetchAnd(Value, MemoryOrder) volatile noexcept
        requires IntegralT<Value>;
    Value FetchXor(Value, MemoryOrder) noexcept
        requires IntegralT<Value>;
    Value FetchXor(Value, MemoryOrder) volatile noexcept
        requires IntegralT<Value>;
    Value FetchOr(Value, MemoryOrder) noexcept
        requires IntegralT<Value>;
    Value FetchOr(Value, MemoryOrder) volatile noexcept
        requires IntegralT<Value>;

    Value FetchAdd(Value, MemoryOrder) noexcept
        requires FloatingT<Value>;
    Value FetchAdd(Value, MemoryOrder) volatile noexcept
        requires FloatingT<Value>;
    Value FetchSub(Value, MemoryOrder) noexcept
        requires FloatingT<Value>;
    Value FetchSub(Value, MemoryOrder) volatile noexcept
        requires FloatingT<Value>;

    Value FetchAdd(OffsetT, MemoryOrder) noexcept
        requires(PointerT<Value> && !FunctionPtrT<T>);
    Value FetchAdd(OffsetT, MemoryOrder) volatile noexcept
        requires(PointerT<Value> && !FunctionPtrT<T>);
    Value FetchSub(OffsetT, MemoryOrder) noexcept
        requires(PointerT<Value> && !FunctionPtrT<T>);
    Value FetchSub(OffsetT, MemoryOrder) volatile noexcept
        requires(PointerT<Value> && !FunctionPtrT<T>);

    void Wait(Value, MemoryOrder) const noexcept;
    void Wait(Value, MemoryOrder) const volatile noexcept;
    void Notify() const noexcept;
    void Notify() const volatile noexcept;
    void NotifyAll() const noexcept;
    void NotifyAll() const volatile noexcept;

    bool IsLockFree() const noexcept;
    bool IsLockFree() const volatile noexcept;
    static constexpr bool IsAlwaysLockFree() noexcept;

    static void ThreadFence(MemoryOrder order) noexcept;
    static void SignalFence(MemoryOrder order) noexcept;

    explicit operator Value() const noexcept;
    explicit operator Value() const volatile noexcept;
    Value operator=(Value) noexcept;
    Value operator=(Value) volatile noexcept;

private:
    Atomic(Atomic const&) = delete;
    Atomic& operator=(Atomic const&) = delete;
    Atomic& operator=(Atomic const&) volatile = delete;
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
inline void Atomic<T>::Store(Value val, MemoryOrder order) noexcept [[diagnose_store(order)]]
{
    __atomic_store(memory::AddressOf(m_Value.value), memory::AddressOf(val),
                   static_cast<int>(order));
}

template <TrivialT T>
inline void Atomic<T>::Store(Value val, MemoryOrder order) volatile noexcept
    [[diagnose_store(order)]]
{
    __atomic_store(memory::AddressOf(m_Value.value), memory::AddressOf(val),
                   static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::Load(MemoryOrder order) const noexcept [[diagnose_load(order)]]
{
    Value result;
    __atomic_load(memory::AddressOf(m_Value.value), memory::AddressOf(result),
                  static_cast<int>(order));
    return result;
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::Load(MemoryOrder order) const volatile noexcept
    [[diagnose_load(order)]]
{
    Value result;
    __atomic_load(memory::AddressOf(m_Value.value), memory::AddressOf(result),
                  static_cast<int>(order));
    return result;
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::Exchange(Value val, MemoryOrder order) noexcept
{
    Value result;
    __atomic_exchange(memory::AddressOf(m_Value.value), memory::AddressOf(val),
                      memory::AddressOf(result), static_cast<int>(order));
    return result;
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::Exchange(Value val, MemoryOrder order) volatile noexcept
{
    Value result;
    __atomic_exchange(memory::AddressOf(m_Value.value), memory::AddressOf(val),
                      memory::AddressOf(result), static_cast<int>(order));
    return result;
}

template <TrivialT T>
inline bool Atomic<T>::CompareExchangeStrong(Value& expected, Value desired, MemoryOrder success,
                                             MemoryOrder failure) noexcept
    [[diagnose_compare_exchange(failure)]]
{
    return __atomic_compare_exchange(memory::AddressOf(m_Value.value), memory::AddressOf(expected),
                                     memory::AddressOf(desired), false, static_cast<int>(success),
                                     static_cast<int>(failure));
}

template <TrivialT T>
inline bool Atomic<T>::CompareExchangeStrong(Value& expected, Value desired, MemoryOrder success,
                                             MemoryOrder failure) volatile noexcept
    [[diagnose_compare_exchange(failure)]]
{
    return __atomic_compare_exchange(memory::AddressOf(m_Value.value), memory::AddressOf(expected),
                                     memory::AddressOf(desired), false, static_cast<int>(success),
                                     static_cast<int>(failure));
}

template <TrivialT T>
inline bool Atomic<T>::CompareExchangeStrong(Value& expected, Value desired,
                                             MemoryOrder order) noexcept
{
    return __atomic_compare_exchange(memory::AddressOf(m_Value.value), memory::AddressOf(expected),
                                     memory::AddressOf(desired), false, static_cast<int>(order),
                                     FailureOrder(order));
}

template <TrivialT T>
inline bool Atomic<T>::CompareExchangeWeak(Value& expected, Value desired, MemoryOrder success,
                                           MemoryOrder failure) noexcept
    [[diagnose_compare_exchange(failure)]]
{
    return __atomic_compare_exchange(memory::AddressOf(m_Value.value), memory::AddressOf(expected),
                                     memory::AddressOf(desired), true, static_cast<int>(success),
                                     static_cast<int>(failure));
}

template <TrivialT T>
inline bool Atomic<T>::CompareExchangeWeak(Value& expected, Value desired, MemoryOrder success,
                                           MemoryOrder failure) volatile noexcept
    [[diagnose_compare_exchange(failure)]]
{
    return __atomic_compare_exchange(memory::AddressOf(m_Value.value), memory::AddressOf(expected),
                                     memory::AddressOf(desired), true, static_cast<int>(success),
                                     static_cast<int>(failure));
}

template <TrivialT T>
inline bool Atomic<T>::CompareExchangeWeak(Value& expected, Value desired,
                                           MemoryOrder order) noexcept
{
    return __atomic_compare_exchange(memory::AddressOf(m_Value.value), memory::AddressOf(expected),
                                     memory::AddressOf(desired), true, static_cast<int>(order),
                                     FailureOrder(order));
}

template <TrivialT T>
inline bool Atomic<T>::CompareExchangeWeak(Value& expected, Value desired,
                                           MemoryOrder order) volatile noexcept
{
    return __atomic_compare_exchange(memory::AddressOf(m_Value.value), memory::AddressOf(expected),
                                     memory::AddressOf(desired), true, static_cast<int>(order),
                                     FailureOrder(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::FetchAdd(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_add(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::FetchAdd(Value val, MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_add(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::FetchSub(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_sub(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::FetchSub(Value val, MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_sub(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::FetchAnd(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_and(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::FetchAnd(Value val, MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_and(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::FetchXor(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_xor(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::FetchXor(Value val, MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_xor(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::FetchOr(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_or(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::FetchOr(Value val, MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_or(memory::AddressOf(m_Value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::FetchAdd(Value value, MemoryOrder order) noexcept
    requires FloatingT<Value>
{
    Value old;
    __atomic_load(memory::AddressOf(m_Value.value), memory::AddressOf(old), __ATOMIC_RELAXED);

    Value tmp = old + value;
    while (!__atomic_compare_exchange(memory::AddressOf(m_Value.value), memory::AddressOf(old),
                                      memory::AddressOf(tmp), true, static_cast<int>(order),
                                      FailureOrder(order))) {
        tmp = old + value;
    }

    return old;
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::FetchAdd(Value value, MemoryOrder order) volatile noexcept
    requires FloatingT<Value>
{
    Value old;
    __atomic_load(memory::AddressOf(m_Value.value), memory::AddressOf(old), __ATOMIC_RELAXED);

    Value tmp = old + value;
    while (!__atomic_compare_exchange(memory::AddressOf(m_Value.value), memory::AddressOf(old),
                                      memory::AddressOf(tmp), true, static_cast<int>(order),
                                      FailureOrder(order))) {
        tmp = old + value;
    }

    return old;
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::FetchSub(Value value, MemoryOrder order) noexcept
    requires FloatingT<Value>
{
    Value old;
    __atomic_load(memory::AddressOf(m_Value.value), memory::AddressOf(old), __ATOMIC_RELAXED);

    Value tmp = old - value;
    while (!__atomic_compare_exchange(memory::AddressOf(m_Value.value), memory::AddressOf(old),
                                      memory::AddressOf(tmp), true, static_cast<int>(order),
                                      FailureOrder(order))) {
        tmp = old - value;
    }

    return old;
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::FetchSub(Value value, MemoryOrder order) volatile noexcept
    requires FloatingT<Value>
{
    Value old;
    __atomic_load(memory::AddressOf(m_Value.value), memory::AddressOf(old), __ATOMIC_RELAXED);

    Value tmp = old - value;
    while (!__atomic_compare_exchange(memory::AddressOf(m_Value.value), memory::AddressOf(old),
                                      memory::AddressOf(tmp), true, static_cast<int>(order),
                                      FailureOrder(order))) {
        tmp = old - value;
    }

    return old;
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::FetchAdd(OffsetT offset, MemoryOrder order) noexcept
    requires(PointerT<Value> && !FunctionPtrT<T>)
{
    OffsetT diff = static_cast<OffsetT>(sizeof(RemovePtrT<Value>));
    return __atomic_fetch_add(memory::AddressOf(m_Value.value), offset * diff,
                              static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::FetchAdd(OffsetT offset, MemoryOrder order) volatile noexcept
    requires(PointerT<Value> && !FunctionPtrT<T>)
{
    OffsetT diff = static_cast<OffsetT>(sizeof(RemovePtrT<Value>));
    return __atomic_fetch_add(memory::AddressOf(m_Value.value), offset * diff,
                              static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::FetchSub(OffsetT offset, MemoryOrder order) noexcept
    requires(PointerT<Value> && !FunctionPtrT<T>)
{
    OffsetT diff = static_cast<OffsetT>(sizeof(RemovePtrT<Value>));
    return __atomic_fetch_sub(memory::AddressOf(m_Value.value), offset * diff,
                              static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::FetchSub(OffsetT offset, MemoryOrder order) volatile noexcept
    requires(PointerT<Value> && !FunctionPtrT<T>)
{
    OffsetT diff = static_cast<OffsetT>(sizeof(RemovePtrT<Value>));
    return __atomic_fetch_sub(memory::AddressOf(m_Value.value), offset * diff,
                              static_cast<int>(order));
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
inline Atomic<T>::operator Value() const noexcept
{
    return Load(MemoryOrder::sequential);
}

template <TrivialT T>
inline Atomic<T>::operator Value() const volatile noexcept
{
    return Load(MemoryOrder::sequential);
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::operator=(Value val) noexcept
{
    return Store(val, MemoryOrder::sequential);
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::operator=(Value val) volatile noexcept
{
    return Store(val, MemoryOrder::sequential);
}

template <TrivialT T>
inline void Atomic<T>::ThreadFence(MemoryOrder order) noexcept
{
    __atomic_thread_fence(static_cast<int>(order));
}

template <TrivialT T>
inline void Atomic<T>::SignalFence(MemoryOrder order) noexcept
{
    __atomic_signal_fence(static_cast<int>(order));
}

} // namespace mini