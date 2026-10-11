module;

#if CLANG // TODO: clang some how manages to skip this diagnostic
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Watomic-alignment"
#endif

#define DIAGNOSE_STORE(order)                                                                                          \
    static_diagnose_error(((order) == __ATOMIC_ACQUIRE || (order) == __ATOMIC_ACQ_REL || (order) == __ATOMIC_CONSUME), \
                          "invalid memory order on atomic store operation")

#define DIAGNOSE_LOAD(order)                                                            \
    static_diagnose_error(((order) == __ATOMIC_RELEASE || (order) == __ATOMIC_ACQ_REL), \
                          "invalid memory order on atomic load operation")

#define DIAGNOSE_COMPARE_EXCHANGE(order)                                                \
    static_diagnose_error(((order) == __ATOMIC_RELEASE || (order) == __ATOMIC_ACQ_REL), \
                          "invalid memory order on atomic exchange operation")

#define DIAGNOSE_WAIT(order)                                                            \
    static_diagnose_error(((order) == __ATOMIC_RELEASE || (order) == __ATOMIC_ACQ_REL), \
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
    typedef Base::Value Value;

private:
    mutable Base m_value;

public:
    constexpr Atomic() noexcept(NoThrowDefaultConstructibleT<T>)
        requires DefaultConstructibleT<T>;
    constexpr Atomic(Value value) noexcept;
    constexpr ~Atomic() noexcept = default;

    void store(Value value, MemoryOrder order) noexcept;
    void store(Value value, MemoryOrder order) volatile noexcept;
    Value load(MemoryOrder order) const noexcept;
    Value load(MemoryOrder order) const volatile noexcept;
    Value exchange(Value value, MemoryOrder order) noexcept;
    Value exchange(Value value, MemoryOrder order) volatile noexcept;

    bool compareExchangeStrong(Value& expected, Value desired, MemoryOrder order) noexcept;
    bool compareExchangeStrong(Value& expected, Value desired, MemoryOrder success, MemoryOrder failure) noexcept;
    bool compareExchangeStrong(Value& expected,
                               Value desired,
                               MemoryOrder success,
                               MemoryOrder failure) volatile noexcept;

    bool compareExchangeWeak(Value& expected, Value desired, MemoryOrder order) noexcept;
    bool compareExchangeWeak(Value& expected, Value desired, MemoryOrder order) volatile noexcept;
    bool compareExchangeWeak(Value& expected, Value desired, MemoryOrder success, MemoryOrder failure) noexcept;
    bool compareExchangeWeak(Value& expected,
                             Value desired,
                             MemoryOrder success,
                             MemoryOrder failure) volatile noexcept;

    Value fetchAdd(Value value, MemoryOrder order) noexcept
        requires IntegralT<Value>;
    Value fetchAdd(Value value, MemoryOrder order) volatile noexcept
        requires IntegralT<Value>;
    Value fetchSub(Value value, MemoryOrder order) noexcept
        requires IntegralT<Value>;
    Value fetchSub(Value value, MemoryOrder order) volatile noexcept
        requires IntegralT<Value>;
    Value fetchAnd(Value value, MemoryOrder order) noexcept
        requires IntegralT<Value>;
    Value fetchAnd(Value value, MemoryOrder order) volatile noexcept
        requires IntegralT<Value>;
    Value fetchXor(Value value, MemoryOrder order) noexcept
        requires IntegralT<Value>;
    Value fetchXor(Value value, MemoryOrder order) volatile noexcept
        requires IntegralT<Value>;
    Value fetchOr(Value value, MemoryOrder order) noexcept
        requires IntegralT<Value>;
    Value fetchOr(Value value, MemoryOrder order) volatile noexcept
        requires IntegralT<Value>;

    Value fetchAdd(Value value, MemoryOrder order) noexcept
        requires FloatingT<Value>;
    Value fetchAdd(Value value, MemoryOrder order) volatile noexcept
        requires FloatingT<Value>;
    Value fetchSub(Value value, MemoryOrder order) noexcept
        requires FloatingT<Value>;
    Value fetchSub(Value value, MemoryOrder order) volatile noexcept
        requires FloatingT<Value>;

    Value fetchAdd(offset_t offset, MemoryOrder order) noexcept
        requires(PointerT<Value> && !FunctionPtrT<T>);
    Value fetchAdd(offset_t offset, MemoryOrder order) volatile noexcept
        requires(PointerT<Value> && !FunctionPtrT<T>);
    Value fetchSub(offset_t offset, MemoryOrder order) noexcept
        requires(PointerT<Value> && !FunctionPtrT<T>);
    Value fetchSub(offset_t offset, MemoryOrder order) volatile noexcept
        requires(PointerT<Value> && !FunctionPtrT<T>);

    void wait(Value old, MemoryOrder order) const noexcept;
    void wait(Value old, MemoryOrder order) const volatile noexcept;
    void notify() const noexcept;
    void notify() const volatile noexcept;
    void notifyAll() const noexcept;
    void notifyAll() const volatile noexcept;

    bool lockFree() const noexcept;
    bool lockFree() const volatile noexcept;
    static constexpr bool alwaysLockFree() noexcept;

    static void threadFence(MemoryOrder order) noexcept;
    static void signalFence(MemoryOrder order) noexcept;

    explicit operator Value() const noexcept;
    explicit operator Value() const volatile noexcept;
    Value operator=(Value value) noexcept;
    Value operator=(Value value) volatile noexcept;

public:
    Atomic(Atomic const&) = delete;
    Atomic& operator=(Atomic const&) = delete;
    Atomic& operator=(Atomic const&) volatile = delete;
};

template <TrivialT T>
constexpr Atomic<T>::Atomic() noexcept(NoThrowDefaultConstructibleT<T>)
    requires DefaultConstructibleT<T>
    : m_value(T())
{
}

template <TrivialT T>
constexpr Atomic<T>::Atomic(Value val) noexcept
    : m_value(val)
{
}

template <TrivialT T>
inline void Atomic<T>::store(Value val, MemoryOrder order) noexcept [[DIAGNOSE_STORE(order)]]
{
    __atomic_store(addressof(m_value.value), addressof(val), static_cast<int>(order));
}

template <TrivialT T>
inline void Atomic<T>::store(Value val, MemoryOrder order) volatile noexcept [[DIAGNOSE_STORE(order)]]
{
    __atomic_store(addressof(m_value.value), addressof(val), static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::load(MemoryOrder order) const noexcept [[DIAGNOSE_LOAD(order)]]
{
    Value result;
    __atomic_load(addressof(m_value.value), addressof(result), static_cast<int>(order));
    return result;
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::load(MemoryOrder order) const volatile noexcept [[DIAGNOSE_LOAD(order)]]
{
    Value result;
    __atomic_load(addressof(m_value.value), addressof(result), static_cast<int>(order));
    return result;
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::exchange(Value val, MemoryOrder order) noexcept
{
    Value result;
    __atomic_exchange(addressof(m_value.value), addressof(val), addressof(result), static_cast<int>(order));
    return result;
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::exchange(Value val, MemoryOrder order) volatile noexcept
{
    Value result;
    __atomic_exchange(addressof(m_value.value), addressof(val), addressof(result), static_cast<int>(order));
    return result;
}

template <TrivialT T>
inline bool Atomic<T>::compareExchangeStrong(Value& expected,
                                             Value desired,
                                             MemoryOrder success,
                                             MemoryOrder failure) noexcept [[DIAGNOSE_COMPARE_EXCHANGE(failure)]]
{
    return __atomic_compare_exchange(addressof(m_value.value),
                                     addressof(expected),
                                     addressof(desired),
                                     false,
                                     static_cast<int>(success),
                                     static_cast<int>(failure));
}

template <TrivialT T>
inline bool Atomic<T>::compareExchangeStrong(Value& expected,
                                             Value desired,
                                             MemoryOrder success,
                                             MemoryOrder failure) volatile noexcept
    [[DIAGNOSE_COMPARE_EXCHANGE(failure)]]
{
    return __atomic_compare_exchange(addressof(m_value.value),
                                     addressof(expected),
                                     addressof(desired),
                                     false,
                                     static_cast<int>(success),
                                     static_cast<int>(failure));
}

template <TrivialT T>
inline bool Atomic<T>::compareExchangeStrong(Value& expected, Value desired, MemoryOrder order) noexcept
{
    return __atomic_compare_exchange(addressof(m_value.value),
                                     addressof(expected),
                                     addressof(desired),
                                     false,
                                     static_cast<int>(order),
                                     failureOrder(order));
}

template <TrivialT T>
inline bool Atomic<T>::compareExchangeWeak(Value& expected,
                                           Value desired,
                                           MemoryOrder success,
                                           MemoryOrder failure) noexcept [[DIAGNOSE_COMPARE_EXCHANGE(failure)]]
{
    return __atomic_compare_exchange(addressof(m_value.value),
                                     addressof(expected),
                                     addressof(desired),
                                     true,
                                     static_cast<int>(success),
                                     static_cast<int>(failure));
}

template <TrivialT T>
inline bool Atomic<T>::compareExchangeWeak(Value& expected,
                                           Value desired,
                                           MemoryOrder success,
                                           MemoryOrder failure) volatile noexcept [[DIAGNOSE_COMPARE_EXCHANGE(failure)]]
{
    return __atomic_compare_exchange(addressof(m_value.value),
                                     addressof(expected),
                                     addressof(desired),
                                     true,
                                     static_cast<int>(success),
                                     static_cast<int>(failure));
}

template <TrivialT T>
inline bool Atomic<T>::compareExchangeWeak(Value& expected, Value desired, MemoryOrder order) noexcept
{
    return __atomic_compare_exchange(addressof(m_value.value),
                                     addressof(expected),
                                     addressof(desired),
                                     true,
                                     static_cast<int>(order),
                                     failureOrder(order));
}

template <TrivialT T>
inline bool Atomic<T>::compareExchangeWeak(Value& expected, Value desired, MemoryOrder order) volatile noexcept
{
    return __atomic_compare_exchange(addressof(m_value.value),
                                     addressof(expected),
                                     addressof(desired),
                                     true,
                                     static_cast<int>(order),
                                     failureOrder(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::fetchAdd(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_add(addressof(m_value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::fetchAdd(Value val, MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_add(addressof(m_value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::fetchSub(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_sub(addressof(m_value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::fetchSub(Value val, MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_sub(addressof(m_value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::fetchAnd(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_and(addressof(m_value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::fetchAnd(Value val, MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_and(addressof(m_value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::fetchXor(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_xor(addressof(m_value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::fetchXor(Value val, MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_xor(addressof(m_value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::fetchOr(Value val, MemoryOrder order) noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_or(addressof(m_value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::fetchOr(Value val, MemoryOrder order) volatile noexcept
    requires IntegralT<Value>
{
    return __atomic_fetch_or(addressof(m_value.value), val, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::fetchAdd(Value value, MemoryOrder order) noexcept
    requires FloatingT<Value>
{
    Value old;
    __atomic_load(addressof(m_value.value), addressof(old), __ATOMIC_RELAXED);

    Value tmp = old + value;
    while (!__atomic_compare_exchange(addressof(m_value.value),
                                      addressof(old),
                                      addressof(tmp),
                                      true,
                                      static_cast<int>(order),
                                      failureOrder(order))) {
        tmp = old + value;
    }

    return old;
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::fetchAdd(Value value, MemoryOrder order) volatile noexcept
    requires FloatingT<Value>
{
    Value old;
    __atomic_load(addressof(m_value.value), addressof(old), __ATOMIC_RELAXED);

    Value tmp = old + value;
    while (!__atomic_compare_exchange(addressof(m_value.value),
                                      addressof(old),
                                      addressof(tmp),
                                      true,
                                      static_cast<int>(order),
                                      failureOrder(order))) {
        tmp = old + value;
    }

    return old;
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::fetchSub(Value value, MemoryOrder order) noexcept
    requires FloatingT<Value>
{
    Value old;
    __atomic_load(addressof(m_value.value), addressof(old), __ATOMIC_RELAXED);

    Value tmp = old - value;
    while (!__atomic_compare_exchange(addressof(m_value.value),
                                      addressof(old),
                                      addressof(tmp),
                                      true,
                                      static_cast<int>(order),
                                      failureOrder(order))) {
        tmp = old - value;
    }

    return old;
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::fetchSub(Value value, MemoryOrder order) volatile noexcept
    requires FloatingT<Value>
{
    Value old;
    __atomic_load(addressof(m_value.value), addressof(old), __ATOMIC_RELAXED);

    Value tmp = old - value;
    while (!__atomic_compare_exchange(addressof(m_value.value),
                                      addressof(old),
                                      addressof(tmp),
                                      true,
                                      static_cast<int>(order),
                                      failureOrder(order))) {
        tmp = old - value;
    }

    return old;
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::fetchAdd(offset_t offset, MemoryOrder order) noexcept
    requires(PointerT<Value> && !FunctionPtrT<T>)
{
    offset_t diff = static_cast<offset_t>(sizeof(RemovePtrT<Value>));
    return __atomic_fetch_add(addressof(m_value.value), offset * diff, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::fetchAdd(offset_t offset, MemoryOrder order) volatile noexcept
    requires(PointerT<Value> && !FunctionPtrT<T>)
{
    offset_t diff = static_cast<offset_t>(sizeof(RemovePtrT<Value>));
    return __atomic_fetch_add(addressof(m_value.value), offset * diff, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::fetchSub(offset_t offset, MemoryOrder order) noexcept
    requires(PointerT<Value> && !FunctionPtrT<T>)
{
    offset_t diff = static_cast<offset_t>(sizeof(RemovePtrT<Value>));
    return __atomic_fetch_sub(addressof(m_value.value), offset * diff, static_cast<int>(order));
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::fetchSub(offset_t offset, MemoryOrder order) volatile noexcept
    requires(PointerT<Value> && !FunctionPtrT<T>)
{
    offset_t diff = static_cast<offset_t>(sizeof(RemovePtrT<Value>));
    return __atomic_fetch_sub(addressof(m_value.value), offset * diff, static_cast<int>(order));
}

template <TrivialT T>
inline void Atomic<T>::wait(Value old, MemoryOrder order) const noexcept [[DIAGNOSE_WAIT(order)]]
{
    __atomic_wait(addressof(m_value.value), old, static_cast<int32>(order));
}

template <TrivialT T>
inline void Atomic<T>::wait(Value old, MemoryOrder order) const volatile noexcept [[DIAGNOSE_WAIT(order)]]
{
    __atomic_wait(addressof(m_value.value), old, static_cast<int32>(order));
}

template <TrivialT T>
inline void Atomic<T>::notify() const noexcept
{
    __atomic_notify_one(addressof(m_value.value));
}

template <TrivialT T>
inline void Atomic<T>::notify() const volatile noexcept
{
    __atomic_notify_one(addressof(m_value.value));
}

template <TrivialT T>
inline void Atomic<T>::notifyAll() const noexcept
{
    __atomic_notify_all(addressof(m_value.value));
}

template <TrivialT T>
inline void Atomic<T>::notifyAll() const volatile noexcept
{
    __atomic_notify_all(addressof(m_value.value));
}

template <TrivialT T>
inline bool Atomic<T>::lockFree() const noexcept
{
    void const* loc = static_cast<void const*>(addressof(m_value.value));
    return __atomic_is_lock_free(sizeof(Base), const_cast<void*>(loc));
}

template <TrivialT T>
inline bool Atomic<T>::lockFree() const volatile noexcept
{
    void const volatile* loc = static_cast<void const volatile*>(addressof(m_value.value));
    return __atomic_is_lock_free(sizeof(Base), const_cast<void*>(loc));
}

template <TrivialT T>
constexpr bool Atomic<T>::alwaysLockFree() noexcept
{
    return __atomic_always_lock_free(sizeof(Base), nullptr);
}

template <TrivialT T>
inline Atomic<T>::operator Value() const noexcept
{
    return load(MemoryOrder::Sequential);
}

template <TrivialT T>
inline Atomic<T>::operator Value() const volatile noexcept
{
    return load(MemoryOrder::Sequential);
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::operator=(Value val) noexcept
{
    return store(val, MemoryOrder::Sequential);
}

template <TrivialT T>
inline Atomic<T>::Value Atomic<T>::operator=(Value val) volatile noexcept
{
    return store(val, MemoryOrder::Sequential);
}

template <TrivialT T>
inline void Atomic<T>::threadFence(MemoryOrder order) noexcept
{
    __atomic_thread_fence(static_cast<int>(order));
}

template <TrivialT T>
inline void Atomic<T>::signalFence(MemoryOrder order) noexcept
{
    __atomic_signal_fence(static_cast<int>(order));
}

} // namespace mini