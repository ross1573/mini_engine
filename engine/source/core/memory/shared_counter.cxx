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
    constexpr SharedCounter(size_t = 1) noexcept;
    constexpr SharedCounter(size_t, size_t) noexcept;
    constexpr virtual ~SharedCounter() = default;

    constexpr size_t Count() const noexcept;
    constexpr size_t WeakCount() const noexcept;

    constexpr void Retain(size_t = 1) noexcept;
    constexpr void RetainWeak(size_t = 1) noexcept;

    constexpr void Release(size_t = 1) noexcept;
    constexpr void ReleaseWeak(size_t = 1) noexcept;

    constexpr SharedCounter* Lock() noexcept;

protected:
    virtual void DeletePtr() noexcept = 0;
    virtual void DeleteSharedBlock() noexcept = 0;

private:
    SharedCounter(SharedCounter const&) = delete;
    SharedCounter(SharedCounter&&) = delete;

    SharedCounter& operator=(SharedCounter const&) = delete;
    SharedCounter& operator=(SharedCounter&&) = delete;
};

inline constexpr SharedCounter::SharedCounter(size_t count) noexcept
    : m_count(static_cast<CounterValue>(count))
    , m_weak(static_cast<CounterValue>(count))
{
}

inline constexpr SharedCounter::SharedCounter(size_t count, size_t weakCount) noexcept
    : m_count(static_cast<CounterValue>(count))
    , m_weak(static_cast<CounterValue>(weakCount))
{
}

inline constexpr size_t SharedCounter::Count() const noexcept
{
    if consteval {
        return static_cast<size_t>(m_count);
    }

    CounterValue result;
    __atomic_load(&m_count, &result, __ATOMIC_RELAXED);
    return static_cast<size_t>(result);
}

inline constexpr size_t SharedCounter::WeakCount() const noexcept
{
    if consteval {
        return static_cast<size_t>(m_weak);
    }

    CounterValue result;
    __atomic_load(&m_weak, &result, __ATOMIC_RELAXED);
    return static_cast<size_t>(result);
}

inline constexpr void SharedCounter::Retain(size_t count) noexcept
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

inline constexpr void SharedCounter::RetainWeak(size_t count) noexcept
{
    CounterValue add = static_cast<CounterValue>(count);

    if consteval {
        m_weak += add;
        return;
    }

    __atomic_fetch_add(&m_weak, add, __ATOMIC_RELAXED);
}

inline constexpr void SharedCounter::Release(size_t count) noexcept
{
    CounterValue sub = static_cast<CounterValue>(count);

    if consteval {
        ASSERT(m_count >= sub, "strong ref count is below zero");

        m_count -= sub;
        if (m_count == 0) {
            DeletePtr();
        }

        ReleaseWeak(count);
        return;
    }

    CounterValue last = __atomic_fetch_sub(&m_count, sub, __ATOMIC_RELEASE);
    if (last == sub) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        DeletePtr();
    } else {
        ASSERT(last > sub, "strong ref count is below zero");
    }

    ReleaseWeak(count);
}

inline constexpr void SharedCounter::ReleaseWeak(size_t count) noexcept
{
    CounterValue sub = static_cast<CounterValue>(count);

    if consteval {
        ASSERT(m_weak >= sub, "weak ref count is below zero");

        m_weak -= sub;
        if (m_weak == 0) {
            DeleteSharedBlock();
        }

        return;
    }

    CounterValue last = __atomic_fetch_sub(&m_weak, sub, __ATOMIC_RELEASE);
    if (last == sub) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        DeleteSharedBlock();
    } else {
        ASSERT(last > sub, "weak ref count is below zero");
    }
}

inline constexpr SharedCounter* SharedCounter::Lock() noexcept
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