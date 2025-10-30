export module mini.core:shared_counter;

import :type;
import :atomic;

namespace mini {

class CORE_API SharedCounter {
private:
    typedef memory::Atomic<int32> Counter;

    Counter m_Count;
    Counter m_Weak;

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
    : m_Count((int32)count)
    , m_Weak((int32)count)
{
}

inline constexpr SharedCounter::SharedCounter(SizeT count, SizeT weakCount) noexcept
    : m_Count((int32)count)
    , m_Weak((int32)weakCount)
{
}

inline constexpr SizeT SharedCounter::Count() const noexcept
{
    return (SizeT)m_Count.Load(memory::MemoryOrder::relaxed);
}

inline constexpr SizeT SharedCounter::WeakCount() const noexcept
{
    return (SizeT)m_Weak.Load(memory::MemoryOrder::relaxed);
}

inline constexpr void SharedCounter::Retain(SizeT count) noexcept
{
    m_Count.FetchAdd((int32)count, memory::MemoryOrder::relaxed);
    m_Weak.FetchAdd((int32)count, memory::MemoryOrder::relaxed);
}

inline constexpr void SharedCounter::RetainWeak(SizeT count) noexcept
{
    m_Weak.FetchAdd((int32)count, memory::MemoryOrder::relaxed);
}

inline constexpr void SharedCounter::Release(SizeT count) noexcept
{
    int32 last = m_Count.FetchSub((int32)count, memory::MemoryOrder::release);
    if (last == (int32)count) {
        Counter::ThreadFence(memory::MemoryOrder::acquire);
        DeletePtr();
    }
    else {
        ASSERT(last > (int32)count, "strong ref count is below zero");
    }

    ReleaseWeak(count);
}

inline constexpr void SharedCounter::ReleaseWeak(SizeT count) noexcept
{
    int32 last = m_Weak.FetchSub((int32)count, memory::MemoryOrder::release);
    if (last == (int32)count) {
        Counter::ThreadFence(memory::MemoryOrder::acquire);
        DeleteSharedBlock();
    }
    else {
        ASSERT(last > (int32)count, "weak ref count is below zero");
    }
}

} // namespace mini