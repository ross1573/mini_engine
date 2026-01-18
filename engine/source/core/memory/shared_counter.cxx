export module mini.core:shared_counter;

import :type;
import :atomic_platform;

namespace mini {

class CORE_API SharedCounter {
private:
    typedef int32 CounterValue;

    CounterValue m_Count;
    CounterValue m_Weak;

public:
    constexpr SharedCounter(SizeT = 1) noexcept;
    constexpr SharedCounter(SizeT, SizeT) noexcept;
    constexpr virtual ~SharedCounter() = default;

    constexpr SizeT Count() const noexcept;
    constexpr SizeT WeakCount() const noexcept;

    constexpr void Retain(SizeT = 1) noexcept;
    constexpr void RetainWeak(SizeT = 1) noexcept;

    constexpr void Release(SizeT = 1) noexcept;
    constexpr void ReleaseWeak(SizeT = 1) noexcept;

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

inline constexpr SharedCounter::SharedCounter(SizeT count) noexcept
    : m_Count(static_cast<CounterValue>(count))
    , m_Weak(static_cast<CounterValue>(count))
{
}

inline constexpr SharedCounter::SharedCounter(SizeT count, SizeT weakCount) noexcept
    : m_Count(static_cast<CounterValue>(count))
    , m_Weak(static_cast<CounterValue>(weakCount))
{
}

inline constexpr SizeT SharedCounter::Count() const noexcept
{
    if consteval {
        return static_cast<SizeT>(m_Count);
    }

    CounterValue result;
    __atomic_load(&m_Count, &result, __ATOMIC_RELAXED);
    return static_cast<SizeT>(result);
}

inline constexpr SizeT SharedCounter::WeakCount() const noexcept
{
    if consteval {
        return static_cast<SizeT>(m_Weak);
    }

    CounterValue result;
    __atomic_load(&m_Weak, &result, __ATOMIC_RELAXED);
    return static_cast<SizeT>(result);
}

inline constexpr void SharedCounter::Retain(SizeT count) noexcept
{
    CounterValue add = static_cast<CounterValue>(count);

    if consteval {
        m_Count += add;
        m_Weak += add;
        return;
    }

    __atomic_fetch_add(&m_Count, add, __ATOMIC_RELAXED);
    __atomic_fetch_add(&m_Weak, add, __ATOMIC_RELAXED);
}

inline constexpr void SharedCounter::RetainWeak(SizeT count) noexcept
{
    CounterValue add = static_cast<CounterValue>(count);

    if consteval {
        m_Weak += add;
        return;
    }

    __atomic_fetch_add(&m_Weak, add, __ATOMIC_RELAXED);
}

inline constexpr void SharedCounter::Release(SizeT count) noexcept
{
    CounterValue sub = static_cast<CounterValue>(count);

    if consteval {
        ASSERT(m_Count >= sub, "strong ref count is below zero");

        m_Count -= sub;
        if (m_Count == 0) {
            DeletePtr();
        }

        ReleaseWeak(count);
        return;
    }

    CounterValue last = __atomic_fetch_sub(&m_Count, sub, __ATOMIC_RELEASE);
    if (last == sub) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        DeletePtr();
    } else {
        ASSERT(last > sub, "strong ref count is below zero");
    }

    ReleaseWeak(count);
}

inline constexpr void SharedCounter::ReleaseWeak(SizeT count) noexcept
{
    CounterValue sub = static_cast<CounterValue>(count);

    if consteval {
        ASSERT(m_Weak >= sub, "weak ref count is below zero");

        m_Weak -= sub;
        if (m_Weak == 0) {
            DeleteSharedBlock();
        }

        return;
    }

    CounterValue last = __atomic_fetch_sub(&m_Weak, sub, __ATOMIC_RELEASE);
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
        if (m_Count == 0) {
            return nullptr;
        }

        ++m_Count;
        ++m_Weak;
        return this;
    }

    CounterValue count;
    __atomic_load(&m_Count, &count, __ATOMIC_RELAXED);
    while (count != 0) {
        CounterValue desired = count + 1;
        if (__atomic_compare_exchange(&m_Count, &count, &desired, true, __ATOMIC_RELAXED,
                                      __ATOMIC_RELAXED)) {
            __atomic_fetch_add(&m_Weak, 1, __ATOMIC_RELAXED);
            return this;
        }
    }

    return nullptr;
}

} // namespace mini