export module mini.core:shared_counter;

import :type;
import :assert;
import :atomic_platform;

namespace mini {

class CORE_API SharedCounter {
private:
    typedef int32 CounterValue;

    CounterValue m_count;
    CounterValue m_weak;

public:
    constexpr SharedCounter(size_t count = 1) noexcept;
    constexpr SharedCounter(size_t count, size_t weakCount) noexcept;
    constexpr virtual ~SharedCounter() = default;

    [[nodiscard]] constexpr size_t count() const noexcept;
    [[nodiscard]] constexpr size_t weakCount() const noexcept;

    constexpr void retain(size_t count = 1) noexcept;
    constexpr void retainWeak(size_t count = 1) noexcept;

    constexpr void release(size_t count = 1) noexcept;
    constexpr void releaseWeak(size_t count = 1) noexcept;

    [[nodiscard]] constexpr SharedCounter* lock() noexcept;

public:
    SharedCounter(SharedCounter const&) = delete;
    SharedCounter(SharedCounter&&) = delete;

    SharedCounter& operator=(SharedCounter const&) = delete;
    SharedCounter& operator=(SharedCounter&&) = delete;

protected:
    virtual void deletePointer() noexcept = 0;
    virtual void deleteSharedBlock() noexcept = 0;
};

constexpr SharedCounter::SharedCounter(size_t count) noexcept
    : m_count(static_cast<CounterValue>(count))
    , m_weak(static_cast<CounterValue>(count))
{
}

constexpr SharedCounter::SharedCounter(size_t count, size_t weakCount) noexcept
    : m_count(static_cast<CounterValue>(count))
    , m_weak(static_cast<CounterValue>(weakCount))
{
}

constexpr size_t SharedCounter::count() const noexcept
{
    if consteval {
        return static_cast<size_t>(m_count);
    }

    CounterValue result;
    __atomic_load(&m_count, &result, __ATOMIC_RELAXED);
    return static_cast<size_t>(result);
}

constexpr size_t SharedCounter::weakCount() const noexcept
{
    if consteval {
        return static_cast<size_t>(m_weak);
    }

    CounterValue result;
    __atomic_load(&m_weak, &result, __ATOMIC_RELAXED);
    return static_cast<size_t>(result);
}

constexpr void SharedCounter::retain(size_t count) noexcept
{
    CounterValue add = static_cast<CounterValue>(count);

    if consteval {
        m_count += add;
        m_weak += add;
        return;
    }

    __atomic_fetch_add(&m_count, add, __ATOMIC_RELAXED);
    __atomic_fetch_add(&m_weak, add, __ATOMIC_RELAXED);
}

constexpr void SharedCounter::retainWeak(size_t count) noexcept
{
    CounterValue add = static_cast<CounterValue>(count);

    if consteval {
        m_weak += add;
        return;
    }

    __atomic_fetch_add(&m_weak, add, __ATOMIC_RELAXED);
}

constexpr void SharedCounter::release(size_t count) noexcept
{
    CounterValue sub = static_cast<CounterValue>(count);

    if consteval {
        ASSERT(m_count >= sub, "strong ref count is below zero");

        m_count -= sub;
        if (m_count == 0) {
            deletePointer();
        }

        releaseWeak(count);
        return;
    }

    CounterValue last = __atomic_fetch_sub(&m_count, sub, __ATOMIC_RELEASE);
    if (last == sub) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        deletePointer();
    } else {
        ASSERT(last > sub, "strong ref count is below zero");
    }

    releaseWeak(count);
}

constexpr void SharedCounter::releaseWeak(size_t count) noexcept
{
    CounterValue sub = static_cast<CounterValue>(count);

    if consteval {
        ASSERT(m_weak >= sub, "weak ref count is below zero");

        m_weak -= sub;
        if (m_weak == 0) {
            deleteSharedBlock();
        }

        return;
    }

    CounterValue last = __atomic_fetch_sub(&m_weak, sub, __ATOMIC_RELEASE);
    if (last == sub) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        deleteSharedBlock();
    } else {
        ASSERT(last > sub, "weak ref count is below zero");
    }
}

constexpr SharedCounter* SharedCounter::lock() noexcept
{
    if consteval {
        if (m_count == 0) {
            return nullptr;
        }

        ++m_count;
        ++m_weak;
        return this;
    }

    CounterValue count;
    __atomic_load(&m_count, &count, __ATOMIC_RELAXED);
    while (count != 0) {
        CounterValue desired = count + 1;
        if (__atomic_compare_exchange(&m_count, &count, &desired, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
            __atomic_fetch_add(&m_weak, 1, __ATOMIC_RELAXED);
            return this;
        }
    }

    return nullptr;
}

} // namespace mini