export module mini.core:atomic_base;

import :type;
import :atomic_platform;

namespace mini {

enum class MemoryOrder : int {
    relaxed = __ATOMIC_RELAXED,
    consume = __ATOMIC_CONSUME,
    acquire = __ATOMIC_ACQUIRE,
    release = __ATOMIC_RELEASE,
    acquireRelease = __ATOMIC_ACQ_REL,
    sequential = __ATOMIC_SEQ_CST
};

inline constexpr int32 FailureOrder(MemoryOrder order)
{
    switch (order) {
        case MemoryOrder::relaxed:
        case MemoryOrder::release:        return __ATOMIC_RELAXED;
        case MemoryOrder::consume:        return __ATOMIC_CONSUME;
        case MemoryOrder::acquire:
        case MemoryOrder::acquireRelease: return __ATOMIC_ACQUIRE;
        case MemoryOrder::sequential:
        default:                          return __ATOMIC_SEQ_CST;
    }
}

template <typename T>
consteval SizeT AtomicAlignment()
{
    if (sizeof(T) > __ATOMIC_MAX_SUPPORT_SIZE) {
        return alignof(T);
    }

    SizeT align = __ATOMIC_MAX_SUPPORT_SIZE;
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

    static constexpr SizeT valueSize = sizeof(Value);
    static constexpr SizeT alignment = AtomicAlignment<Value>();

    alignas(alignment) Value value;
};

} // namespace mini