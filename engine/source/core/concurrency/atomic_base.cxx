export module mini.core:atomic_base;

import :type;
import :atomic_platform;

namespace mini {

// NOLINTNEXTLINE: gcc and clang builtin parameter is int
export enum class MemoryOrder : int {
    Relaxed = __ATOMIC_RELAXED,
    Consume = __ATOMIC_CONSUME,
    Acquire = __ATOMIC_ACQUIRE,
    Release = __ATOMIC_RELEASE,
    AcquireRelease = __ATOMIC_ACQ_REL,
    Sequential = __ATOMIC_SEQ_CST,
};

constexpr int32 failureOrder(MemoryOrder order)
{
    switch (order) {
        case MemoryOrder::Relaxed:
        case MemoryOrder::Release:        return __ATOMIC_RELAXED;
        case MemoryOrder::Consume:        return __ATOMIC_CONSUME;
        case MemoryOrder::Acquire:
        case MemoryOrder::AcquireRelease: return __ATOMIC_ACQUIRE;
        case MemoryOrder::Sequential:
        default:                          return __ATOMIC_SEQ_CST;
    }
}

template <typename T>
consteval size_t atomicAlignment()
{
    if (sizeof(T) > __ATOMIC_MAX_SUPPORT_SIZE) {
        return alignof(T);
    }

    size_t align = __ATOMIC_MAX_SUPPORT_SIZE;
    while (align > 0) {
        if ((align >> 1) < sizeof(T)) {
            return align;
        }

        align >>= 1;
    }

    return 1;
}

template <typename T>
struct AtomicBase {
public:
    typedef T Value;

    static_assert(sizeof(Value) != 0, "cannot create atomic value from incomplete type.");

    static constexpr size_t valueSize = sizeof(Value);
    static constexpr size_t alignment = atomicAlignment<Value>();

    alignas(alignment) Value value;
};

} // namespace mini