module;

#include <intrin.h>

#include "memory/cstring.h"

export module mini.core:atomic_platform;

import :type;
import :ignore;
import :memory_operation;

using namespace mini;

export constexpr int32 __ATOMIC_RELAXED = 0;
export constexpr int32 __ATOMIC_CONSUME = 1;
export constexpr int32 __ATOMIC_ACQUIRE = 2;
export constexpr int32 __ATOMIC_RELEASE = 3;
export constexpr int32 __ATOMIC_ACQ_REL = 4;
export constexpr int32 __ATOMIC_SEQ_CST = 5;

#if ARCH_X86_64 || ARCH_ARM64
export constexpr int32 __ATOMIC_INTERFERENCE_SIZE = 64;
export constexpr int32 __ATOMIC_MAX_SUPPORT_SIZE = 16;

struct alignas(16) Packed128 {
public:
    mini::int64 low;
    mini::int64 high;
};

using AtomicLock = uint64;
#elif ARCH_X86_32
export constexpr int32 __ATOMIC_INTERFERENCE_SIZE = 32;
export constexpr int32 __ATOMIC_MAX_SUPPORT_SIZE = 8;

using AtomicLock = uint32;
#endif // Packed128

#if ARCH_ARM64
#  define PAUSE() __isb(_ARM64_BARRIER_SY)
#elif ARCH_X86
#  define PAUSE() _mm_pause()
#else
#  define PAUSE() __nop()
#endif

#define SIGNAL_FENCE()                 \
    _Pragma("warning(push)");          \
    _Pragma("warning(disable: 4996)"); \
    _ReadWriteBarrier();               \
    _Pragma("warning(pop)")

#if ARCH_X86
#  define THREAD_FENCE_ASM()
#  define THREAD_FENCE(memorder)          \
      SIGNAL_FENCE();                     \
      if (memorder == __ATOMIC_SEQ_CST) { \
          volatile long guard;            \
          _InterlockedIncrement(&guard);  \
          SIGNAL_FENCE();                 \
      }
#elif ARCH_ARM64
#  define THREAD_FENCE_ASM()         __dmb(0xB)
#  define THREAD_FENCE_ACQUIRE_ASM() __dmb(0x9)
#  define THREAD_FENCE(memorder)                                          \
      if (memorder == __ATOMIC_ACQUIRE || memorder == __ATOMIC_CONSUME) { \
          THREAD_FENCE_ACQUIRE_ASM();                                     \
      } else {                                                            \
          THREAD_FENCE_ASM();                                             \
      }
#endif // THREAD_FENCE

#define VOLATILE_LOAD(size, pointer, result)                                                      \
    auto opRet = __iso_volatile_load##size(reinterpret_cast<int##size const volatile*>(pointer)); \
    *result = *reinterpret_cast<decltype(result)>(memory::AddressOf(opRet))

#define VOLATILE_STORE(size, pointer, value)                                   \
    __iso_volatile_store##size(reinterpret_cast<int##size volatile*>(pointer), \
                               *reinterpret_cast<int##size*>(value))

#if ARCH_X86
#  define EXCHANGE_ACQUIRE(pointer, value)                                   \
      _InterlockedExchange(reinterpret_cast<long volatile*>(pointer), value)

#  define STORE_SEQ_CST(size, pointer, value)                                        \
      ignore = _InterlockedExchange##size(reinterpret_cast<long volatile*>(pointer), \
                                          *reinterpret_cast<long*>(value))

#  define STORE_RELEASE(size, pointer, value) \
      SIGNAL_FENCE();                         \
      VOLATILE_STORE(size, pointer, value)
#elif ARCH_ARM64
#  define EXCHANGE_ACQUIRE(pointer, value)                                       \
      _InterlockedExchange_acq(reinterpret_cast<long volatile*>(pointer), value)

#  define STORE_RELEASE(size, pointer, value)                                                     \
      __stlr##size(reinterpret_cast<uint##size volatile*>(pointer),                               \
                   *const_cast<uint##size*>(reinterpret_cast<uint##size const volatile*>(value)))
#endif // STORE

#if ARCH_ARM64
#  define LOAD_ACQUIRE(size, pointer, result)                                                   \
      auto opRet = __load_acquire##size(reinterpret_cast<uint##size const volatile*>(pointer)); \
      *result = *reinterpret_cast<decltype(result)>(memory::AddressOf(opRet))
#endif // LOAD

#if ARCH_X86
#  define ATOMIC_BUILTIN_IMPL(type, op, result, memorder, ...)                \
      type opRet = op(__VA_ARGS__);                                           \
      *result = *reinterpret_cast<decltype(result)>(memory::AddressOf(opRet))
#elif ARCH_ARM64
#  define ATOMIC_BUILTIN_IMPL(type, op, result, memorder, ...)                \
      type opRet;                                                             \
      switch (memorder) {                                                     \
          case __ATOMIC_RELAXED: opRet = op##_nf(__VA_ARGS__); break;         \
          case __ATOMIC_ACQUIRE:                                              \
          case __ATOMIC_CONSUME: opRet = op##_acq(__VA_ARGS__); break;        \
          case __ATOMIC_RELEASE: opRet = op##_rel(__VA_ARGS__); break;        \
          default:                                                            \
          case __ATOMIC_ACQ_REL:                                              \
          case __ATOMIC_SEQ_CST: opRet = op(__VA_ARGS__); break;              \
      }                                                                       \
      *result = *reinterpret_cast<decltype(result)>(memory::AddressOf(opRet))
#endif // ATOMIC_BUILTIN_IMPL

#if ARCH_X86_64
#  define ATOMIC_BUILTIN_128_IMPL(op, memorder, ...) mini::ignore = op(__VA_ARGS__);
#elif ARCH_ARM64
#  define ATOMIC_BUILTIN_128_IMPL(op, memorder, ...)                    \
      switch (memorder) {                                               \
          case __ATOMIC_RELAXED: ignore = op##_nf(__VA_ARGS__); break;  \
          case __ATOMIC_ACQUIRE:                                        \
          case __ATOMIC_CONSUME: ignore = op##_acq(__VA_ARGS__); break; \
          case __ATOMIC_RELEASE: ignore = op##_rel(__VA_ARGS__); break; \
          default:                                                      \
          case __ATOMIC_ACQ_REL:                                        \
          case __ATOMIC_SEQ_CST: ignore = op(__VA_ARGS__); break;       \
      }
#endif // ATOMIC_BUILTIN_128_IMPL

#define ATOMIC_BUILTIN(op, size, result, memorder, ...)                     \
    ATOMIC_BUILTIN_IMPL(int##size, op##size, result, memorder, __VA_ARGS__)
#define ATOMIC_BUILTIN_32(op, result, memorder, ...)             \
    ATOMIC_BUILTIN_IMPL(long, op, result, memorder, __VA_ARGS__)

#if ARCH_X86
#  define ATOMIC_LOAD(size, pointer, result, memorder) \
      VOLATILE_LOAD(size, pointer, result);            \
      if (memorder != __ATOMIC_RELAXED) {              \
          SIGNAL_FENCE();                              \
      }
#elif ARCH_ARM64
#  define ATOMIC_LOAD(size, pointer, result, memorder) \
      if (memorder == __ATOMIC_RELAXED) {              \
          VOLATILE_LOAD(size, pointer, result);        \
      } else {                                         \
          LOAD_ACQUIRE(size, pointer, result);         \
          SIGNAL_FENCE();                              \
      }
#endif // ATOMIC_LOAD

#if ARCH_X86
#  define ATOMIC_STORE(size, pointer, value, memorder) \
      if (memorder == __ATOMIC_RELAXED) {              \
          VOLATILE_STORE(size, pointer, value);        \
      } else if (memorder == __ATOMIC_RELEASE) {       \
          SIGNAL_FENCE();                              \
          VOLATILE_STORE(size, pointer, value);        \
      } else {                                         \
          STORE_SEQ_CST(size, pointer, value);         \
      }
#elif ARCH_ARM64
#  define ATOMIC_STORE(size, pointer, value, memorder) \
      if (memorder == __ATOMIC_RELAXED) {              \
          VOLATILE_STORE(size, pointer, value);        \
      } else if (memorder == __ATOMIC_RELEASE) {       \
          SIGNAL_FENCE();                              \
          STORE_RELEASE(size, pointer, value);         \
      } else {                                         \
          SIGNAL_FENCE();                              \
          STORE_RELEASE(size, pointer, value);         \
          THREAD_FENCE_ASM();                          \
      }
#endif // ATOMIC_STORE

#define ATOMIC_EXCHANGE(size, pointer, desired, result, memorder)  \
    ATOMIC_BUILTIN(_InterlockedExchange, size, result, memorder,   \
                   reinterpret_cast<int##size volatile*>(pointer), \
                   *reinterpret_cast<int##size*>(desired))

#define ATOMIC_EXCHANGE_32(pointer, desired, result, memorder)   \
    ATOMIC_BUILTIN_32(_InterlockedExchange, result, memorder,    \
                      reinterpret_cast<long volatile*>(pointer), \
                      *reinterpret_cast<long*>(desired))

#define ATOMIC_COMPARE_EXCHANGE(size, pointer, expected, desired, result, memorder) \
    ATOMIC_BUILTIN(_InterlockedCompareExchange, size, result, memorder,             \
                   reinterpret_cast<int##size volatile*>(pointer),                  \
                   *reinterpret_cast<int##size*>(desired),                          \
                   *reinterpret_cast<int##size*>(expected))

#define ATOMIC_COMPARE_EXCHANGE_32(pointer, expected, desired, result, memorder)             \
    ATOMIC_BUILTIN_32(_InterlockedCompareExchange, result, memorder,                         \
                      reinterpret_cast<long volatile*>(pointer),                             \
                      *reinterpret_cast<long*>(desired), *reinterpret_cast<long*>(expected))

#define ATOMIC_FETCH_ADD(size, pointer, opValue, previous, memorder)  \
    ATOMIC_BUILTIN(_InterlockedExchangeAdd, size, previous, memorder, \
                   reinterpret_cast<int##size volatile*>(pointer),    \
                   *reinterpret_cast<int##size*>(opValue))

#define ATOMIC_FETCH_ADD_32(pointer, opValue, previous, memorder)  \
    ATOMIC_BUILTIN_32(_InterlockedExchangeAdd, previous, memorder, \
                      reinterpret_cast<long volatile*>(pointer),   \
                      *reinterpret_cast<long*>(opValue))

#define ATOMIC_FETCH_AND(size, pointer, value, previous, memorder) \
    ATOMIC_BUILTIN(_InterlockedAnd, size, previous, memorder,      \
                   reinterpret_cast<int##size volatile*>(pointer), \
                   *reinterpret_cast<int##size*>(value))

#define ATOMIC_FETCH_AND_32(pointer, opValue, previous, memorder) \
    ATOMIC_BUILTIN_32(_InterlockedAnd, previous, memorder,        \
                      reinterpret_cast<long volatile*>(pointer),  \
                      *reinterpret_cast<long*>(opValue))

#define ATOMIC_FETCH_OR(size, pointer, opValue, previous, memorder) \
    ATOMIC_BUILTIN(_InterlockedOr, size, previous, memorder,        \
                   reinterpret_cast<int##size volatile*>(pointer),  \
                   *reinterpret_cast<int##size*>(opValue))

#define ATOMIC_FETCH_OR_32(pointer, opValue, previous, memorder) \
    ATOMIC_BUILTIN_32(_InterlockedOr, previous, memorder,        \
                      reinterpret_cast<long volatile*>(pointer), \
                      *reinterpret_cast<long*>(opValue))

#define ATOMIC_FETCH_XOR(size, pointer, opValue, previous, memorder) \
    ATOMIC_BUILTIN(_InterlockedXor, size, previous, memorder,        \
                   reinterpret_cast<int##size volatile*>(pointer),   \
                   *reinterpret_cast<int##size*>(opValue))

#define ATOMIC_FETCH_XOR_32(pointer, opValue, previous, memorder) \
    ATOMIC_BUILTIN_32(_InterlockedXor, previous, memorder,        \
                      reinterpret_cast<long volatile*>(pointer),  \
                      *reinterpret_cast<long*>(opValue))

#if ARCH_X86_64 || ARCH_ARM64
#  define ATOMIC_LOAD_128(pointer, result, memorder)                                \
      ATOMIC_BUILTIN_128_IMPL(_InterlockedCompareExchange128, memorder,             \
                              reinterpret_cast<long long volatile*>(pointer), 0, 0, \
                              reinterpret_cast<long*>(result))

#  define ATOMIC_COMPARE_EXCHANGE_128(pointer, expected, desired, result, memorder)     \
      ATOMIC_BUILTIN_IMPL(mini::byte, _InterlockedCompareExchange128, result, memorder, \
                          reinterpret_cast<long long volatile*>(pointer),               \
                          reinterpret_cast<Packed128*>(desired)->high,                  \
                          reinterpret_cast<Packed128*>(desired)->low,                   \
                          reinterpret_cast<long*>(expected))
#endif

constexpr SizeT lockTableSize = 1 << 8;
CORE_API AtomicLock g_atomicLockTable[lockTableSize];

template <typename T>
struct AtomicSpinLock {
private:
    AtomicLock* m_lock;

public:
    AtomicSpinLock(T const volatile* pointer) noexcept
    {
        SizeT intptr = reinterpret_cast<SizeT>(pointer);
        SizeT hash = intptr >> 6;
        hash ^= hash >> 16;
        hash &= (lockTableSize - 1);
        m_lock = reinterpret_cast<AtomicLock*>(&g_atomicLockTable[hash]);

        while (EXCHANGE_ACQUIRE(m_lock, 1) != 0) {
            auto volatilePtr = reinterpret_cast<int const volatile*>(m_lock);
            while (__iso_volatile_load32(volatilePtr) != 0) {
                PAUSE();
            }
        }
    }

    ~AtomicSpinLock() noexcept
    {
        AtomicLock const value = 0;
        STORE_RELEASE(32, m_lock, &value);
    }
};

template <typename T>
consteval bool IsAtomicSupported()
{
    if (sizeof(T) > __ATOMIC_MAX_SUPPORT_SIZE) {
        return false;
    }

    SizeT size = __ATOMIC_MAX_SUPPORT_SIZE;
    while (size > 0) {
        if (size == sizeof(T)) {
            return true;
        }

        size >>= 1;
    }

    return false;
}

inline CORE_API constexpr int32 CompareExchangeOrder(int32 success, int32 failure)
{
    switch (success) {
        case __ATOMIC_RELAXED: return failure;
        case __ATOMIC_CONSUME:
        case __ATOMIC_ACQUIRE:
            switch (failure) {
                case __ATOMIC_RELAXED:
                case __ATOMIC_CONSUME:
                case __ATOMIC_ACQUIRE: return __ATOMIC_ACQUIRE;
                case __ATOMIC_RELEASE:
                case __ATOMIC_ACQ_REL: return __ATOMIC_ACQ_REL;
                case __ATOMIC_SEQ_CST:
                default:               return __ATOMIC_SEQ_CST;
            }
        case __ATOMIC_RELEASE:
            switch (failure) {
                case __ATOMIC_RELAXED:
                case __ATOMIC_RELEASE: return __ATOMIC_RELEASE;
                case __ATOMIC_CONSUME:
                case __ATOMIC_ACQUIRE:
                case __ATOMIC_ACQ_REL: return __ATOMIC_ACQ_REL;
                case __ATOMIC_SEQ_CST:
                default:               return __ATOMIC_SEQ_CST;
            }
        case __ATOMIC_ACQ_REL:
            return failure == __ATOMIC_SEQ_CST ? __ATOMIC_SEQ_CST : __ATOMIC_ACQ_REL;
        case __ATOMIC_SEQ_CST:
        default:               return __ATOMIC_SEQ_CST;
    }
}

export template <typename T>
    requires(sizeof(T) == 1)
inline bool __atomic_compare_exchange_1(T volatile* pointer, T* expected, T desired, bool /*weak*/,
                                        int32 success, int32 failure)
{
    int32 memorder = CompareExchangeOrder(success, failure);
    T previous;

    ATOMIC_COMPARE_EXCHANGE(8, pointer, expected, memory::AddressOf(desired),
                            memory::AddressOf(previous), memorder);

    if (BUILTIN_MEMCMP(memory::AddressOf(previous), expected, sizeof(T)) == 0) {
        return true;
    }

    BUILTIN_MEMCPY(expected, memory::AddressOf(previous), sizeof(T));
    return false;
}

export template <typename T>
    requires(sizeof(T) == 2)
inline bool __atomic_compare_exchange_2(T volatile* pointer, T* expected, T desired, bool /*weak*/,
                                        int32 success, int32 failure)
{
    int32 memorder = CompareExchangeOrder(success, failure);
    T previous;

    ATOMIC_COMPARE_EXCHANGE(16, pointer, expected, memory::AddressOf(desired),
                            memory::AddressOf(previous), memorder);

    if (BUILTIN_MEMCMP(memory::AddressOf(previous), expected, sizeof(T)) == 0) {
        return true;
    }

    BUILTIN_MEMCPY(expected, memory::AddressOf(previous), sizeof(T));
    return false;
}

export template <typename T>
    requires(sizeof(T) == 4)
inline bool __atomic_compare_exchange_4(T volatile* pointer, T* expected, T desired, bool /*weak*/,
                                        int32 success, int32 failure)
{
    int32 memorder = CompareExchangeOrder(success, failure);
    T previous;

    ATOMIC_COMPARE_EXCHANGE_32(pointer, expected, memory::AddressOf(desired),
                               memory::AddressOf(previous), memorder);

    if (BUILTIN_MEMCMP(memory::AddressOf(previous), expected, sizeof(T)) == 0) {
        return true;
    }

    BUILTIN_MEMCPY(expected, memory::AddressOf(previous), sizeof(T));
    return false;
}

export template <typename T>
    requires(sizeof(T) == 8)
inline bool __atomic_compare_exchange_8(T volatile* pointer, T* expected, T desired, bool /*weak*/,
                                        int32 success, int32 failure)
{
    int32 memorder = CompareExchangeOrder(success, failure);
    T previous;

    ATOMIC_COMPARE_EXCHANGE(64, pointer, expected, memory::AddressOf(desired),
                            memory::AddressOf(previous), memorder);

    if (BUILTIN_MEMCMP(memory::AddressOf(previous), expected, sizeof(T)) == 0) {
        return true;
    }

    BUILTIN_MEMCPY(expected, memory::AddressOf(previous), sizeof(T));
    return false;
}

export template <typename T>
    requires(sizeof(T) == 16)
inline bool __atomic_compare_exchange_16(T volatile* pointer, T* expected, T desired, bool /*weak*/,
                                         int32 success, int32 failure)
{
    int32 memorder = CompareExchangeOrder(success, failure);
    byte result;

    ATOMIC_COMPARE_EXCHANGE_128(pointer, expected, memory::AddressOf(desired), result, memorder);
    if (result == 0) {
        BUILTIN_MEMCPY(expected, memory::AddressOf(desired), sizeof(T));
    }

    return result != 0;
}

export template <typename T>
    requires(IsAtomicSupported<T>())
inline bool __atomic_compare_exchange(T volatile* pointer, T* expected, T* desired, bool /*weak*/,
                                      int32 success, int32 failure)
{
    int32 memorder = CompareExchangeOrder(success, failure);

    constexpr SizeT size = sizeof(T);
    if constexpr (size == 16) {
        byte result;

        ATOMIC_COMPARE_EXCHANGE_128(pointer, expected, desired, &result, memorder);
        if (result == 0) {
            BUILTIN_MEMCPY(expected, desired, sizeof(T));
        }

        return result != 0;
    } else {
        T previous;
        T* prevPtr = memory::AddressOf(previous);

        if constexpr (size == 1) {
            ATOMIC_COMPARE_EXCHANGE(8, pointer, expected, desired, prevPtr, memorder);
        } else if constexpr (size == 2) {
            ATOMIC_COMPARE_EXCHANGE(16, pointer, expected, desired, prevPtr, memorder);
        } else if constexpr (size == 4) {
            ATOMIC_COMPARE_EXCHANGE_32(pointer, expected, desired, prevPtr, memorder);
        } else if constexpr (size == 8) {
            ATOMIC_COMPARE_EXCHANGE(64, pointer, expected, desired, prevPtr, memorder);
        } else {
            NEVER_CALLED("atomic type is not marked as unsupported", T);
        }

        if (BUILTIN_MEMCMP(prevPtr, expected, sizeof(T)) == 0) {
            return true;
        }

        BUILTIN_MEMCPY(expected, prevPtr, sizeof(T));
        return false;
    }
}

export template <typename T>
    requires(!IsAtomicSupported<T>())
inline bool __atomic_compare_exchange(T volatile* pointer, T* expected, T* desired, bool /*weak*/,
                                      int32 /*success*/, int32 /*failure*/)
{
    constexpr SizeT size = sizeof(T);
    AtomicSpinLock lock(pointer);

    if (BUILTIN_MEMCMP(const_cast<T*>(pointer), expected, size) == 0) {
        BUILTIN_MEMCPY(const_cast<T*>(pointer), desired, size);
        return true;
    }

    BUILTIN_MEMCPY(expected, const_cast<T*>(pointer), size);
    return false;
}

export template <typename T>
    requires(sizeof(T) == 1)
inline T __atomic_load_1(T const volatile* pointer, int32 memorder)
{
    T result;
    ATOMIC_LOAD(8, pointer, memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 2)
inline T __atomic_load_2(T const volatile* pointer, int32 memorder)
{
    T result;
    ATOMIC_LOAD(16, pointer, memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 4)
inline T __atomic_load_4(T const volatile* pointer, int32 memorder)
{
    T result;
    ATOMIC_LOAD(32, pointer, memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 8)
inline T __atomic_load_8(T const volatile* pointer, int32 memorder)
{
    T result;
    ATOMIC_LOAD(64, pointer, memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 16)
inline T __atomic_load_16(T const volatile* pointer, int32 memorder)
{
    T result;
    ATOMIC_LOAD_128(pointer, memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
inline void __atomic_load(T const volatile* pointer, T* result, int32 memorder)
{
    constexpr SizeT size = sizeof(T);
    if constexpr (size == 1) {
        ATOMIC_LOAD(8, pointer, result, memorder);
    } else if constexpr (size == 2) {
        ATOMIC_LOAD(16, pointer, result, memorder);
    } else if constexpr (size == 4) {
        ATOMIC_LOAD(32, pointer, result, memorder);
    } else if constexpr (size == 8) {
        ATOMIC_LOAD(64, pointer, result, memorder);
    } else if constexpr (size == 16) {
        ATOMIC_LOAD_128(pointer, result, memorder);
    } else {
        AtomicSpinLock lock(pointer);
        BUILTIN_MEMCPY(result, const_cast<T*>(pointer), size);
    }
}

export template <typename T>
    requires(sizeof(T) == 1)
inline void __atomic_store_1(T volatile* pointer, T value, int32 memorder)
{
    ATOMIC_STORE(8, pointer, memory::AddressOf(value), memorder);
}

export template <typename T>
    requires(sizeof(T) == 2)
inline void __atomic_store_2(T volatile* pointer, T value, int32 memorder)
{
    ATOMIC_STORE(16, pointer, memory::AddressOf(value), memorder);
}

export template <typename T>
    requires(sizeof(T) == 4)
inline void __atomic_store_4(T volatile* pointer, T value, int32 memorder)
{
    ATOMIC_STORE(32, pointer, memory::AddressOf(value), memorder);
}

export template <typename T>
    requires(sizeof(T) == 8)
inline void __atomic_store_8(T volatile* pointer, T value, int32 memorder)
{
    ATOMIC_STORE(64, pointer, memory::AddressOf(value), memorder);
}

export template <typename T>
    requires(sizeof(T) == 8)
inline void __atomic_store_16(T volatile* pointer, T value, int32 memorder)
{
    T result{ value };
    while (!__atomic_compare_exchange_16(pointer, memory::AddressOf(result),
                                         memory::AddressOf(value), true, memorder, memorder)) {}
}

export template <typename T>
inline void __atomic_store(T volatile* pointer, T* value, int32 memorder)
{
    constexpr SizeT size = sizeof(T);
    if constexpr (size == 1) {
        ATOMIC_STORE(8, pointer, value, memorder);
    } else if constexpr (size == 2) {
        ATOMIC_STORE(16, pointer, value, memorder);
    } else if constexpr (size == 4) {
        ATOMIC_STORE(32, pointer, value, memorder);
    } else if constexpr (size == 8) {
        ATOMIC_STORE(64, pointer, value, memorder);
    } else if constexpr (size == 16) {
        __atomic_store_16(pointer, *value, memorder);
    } else {
        AtomicSpinLock lock(pointer);
        BUILTIN_MEMCPY(const_cast<T*>(pointer), value, size);
    }
}

export template <typename T>
    requires(sizeof(T) == 1)
inline T __atomic_exchange_1(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_EXCHANGE(8, pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 2)
inline T __atomic_exchange_2(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_EXCHANGE(16, pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 4)
inline T __atomic_exchange_4(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_EXCHANGE_32(pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 8)
inline T __atomic_exchange_8(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_EXCHANGE(64, pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 16)
inline T __atomic_exchange_16(T volatile* pointer, T value, int32 memorder)
{
    T result{ value };
    while (__atomic_compare_exchange_16(pointer, memory::AddressOf(result),
                                        memory::AddressOf(value), true, memorder, memorder)) {}
    return result;
}

export template <typename T>
inline void __atomic_exchange(T volatile* pointer, T* value, T* result, int32 memorder)
{
    constexpr SizeT size = sizeof(T);
    if constexpr (size == 1) {
        ATOMIC_EXCHANGE(8, pointer, value, result, memorder);
    } else if constexpr (size == 2) {
        ATOMIC_EXCHANGE(16, pointer, value, result, memorder);
    } else if constexpr (size == 4) {
        ATOMIC_EXCHANGE_32(pointer, value, result, memorder);
    } else if constexpr (size == 8) {
        ATOMIC_EXCHANGE(64, pointer, value, result, memorder);
    } else if constexpr (size == 16) {
        *result = __atomic_exchange_16(pointer, *value, memorder);
    } else {
        AtomicSpinLock lock(pointer);
        BUILTIN_MEMCPY(result, const_cast<T*>(pointer), size);
        BUILTIN_MEMCPY(const_cast<T*>(pointer), value, size);
    }
}

export template <typename T>
    requires(sizeof(T) == 1)
inline T __atomic_fetch_add_1(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_FETCH_ADD(8, pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 2)
inline T __atomic_fetch_add_2(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_FETCH_ADD(16, pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 4)
inline T __atomic_fetch_add_4(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_FETCH_ADD_32(pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 8)
inline T __atomic_fetch_add_8(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_FETCH_ADD(64, pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
inline T __atomic_fetch_add(T volatile* pointer, T value, int32 memorder)
{
    T result;
    T* resultPtr = memory::AddressOf(result);
    T* valuePtr = memory::AddressOf(value);

    constexpr SizeT size = sizeof(T);
    if constexpr (size == 1) {
        ATOMIC_FETCH_ADD(8, pointer, valuePtr, resultPtr, memorder);
    } else if constexpr (size == 2) {
        ATOMIC_FETCH_ADD(16, pointer, valuePtr, resultPtr, memorder);
    } else if constexpr (size == 4) {
        ATOMIC_FETCH_ADD_32(pointer, valuePtr, resultPtr, memorder);
    } else if constexpr (size == 8) {
        ATOMIC_FETCH_ADD(64, pointer, valuePtr, resultPtr, memorder);
    } else {
        NEVER_CALLED("invalid integral operation", T);
    }

    return result;
}

export template <PointerT T>
inline T __atomic_fetch_add(T volatile* pointer, OffsetT value, int32 memorder)
    requires(!FunctionPtrT<T>)
{
    T result;
    T* resultPtr = memory::AddressOf(result);
    OffsetT* valuePtr = memory::AddressOf(value);

    constexpr SizeT size = sizeof(T);
    if constexpr (size == 4) {
        ATOMIC_FETCH_ADD_32(pointer, valuePtr, resultPtr, memorder);
    } else if constexpr (size == 8) {
        ATOMIC_FETCH_ADD(64, pointer, valuePtr, resultPtr, memorder);
    }

    return result;
}

export template <typename T>
    requires(sizeof(T) == 1)
inline T __atomic_fetch_sub_1(T volatile* pointer, T value, int32 memorder)
{
    value = ~value + 1;
    T result;
    ATOMIC_FETCH_ADD(8, pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 2)
inline T __atomic_fetch_sub_2(T volatile* pointer, T value, int32 memorder)
{
    value = ~value + 1;
    T result;
    ATOMIC_FETCH_ADD(16, pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 4)
inline T __atomic_fetch_sub_4(T volatile* pointer, T value, int32 memorder)
{
    value = ~value + 1;
    T result;
    ATOMIC_FETCH_ADD_32(pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 8)
inline T __atomic_fetch_sub_8(T volatile* pointer, T value, int32 memorder)
{
    value = ~value + 1;
    T result;
    ATOMIC_FETCH_ADD(64, pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
inline T __atomic_fetch_sub(T volatile* pointer, T value, int32 memorder)
{
    value = ~value + 1;
    T result;
    T* resultPtr = memory::AddressOf(result);
    T* valuePtr = memory::AddressOf(value);

    constexpr SizeT size = sizeof(T);
    if constexpr (size == 1) {
        ATOMIC_FETCH_ADD(8, pointer, valuePtr, resultPtr, memorder);
    } else if constexpr (size == 2) {
        ATOMIC_FETCH_ADD(16, pointer, valuePtr, resultPtr, memorder);
    } else if constexpr (size == 4) {
        ATOMIC_FETCH_ADD_32(pointer, valuePtr, resultPtr, memorder);
    } else if constexpr (size == 8) {
        ATOMIC_FETCH_ADD(64, pointer, valuePtr, resultPtr, memorder);
    } else {
        NEVER_CALLED("invalid integral operation", T);
    }

    return result;
}

export template <PointerT T>
inline T __atomic_fetch_sub(T volatile* pointer, OffsetT value, int32 memorder)
    requires(!FunctionPtrT<T>)
{
    value = ~value + 1;
    T result;
    T* resultPtr = memory::AddressOf(result);
    OffsetT* valuePtr = memory::AddressOf(value);

    constexpr SizeT size = sizeof(T);
    if constexpr (size == 4) {
        ATOMIC_FETCH_ADD_32(pointer, valuePtr, resultPtr, memorder);
    } else if constexpr (size == 8) {
        ATOMIC_FETCH_ADD(64, pointer, valuePtr, resultPtr, memorder);
    }

    return result;
}

export template <typename T>
    requires(sizeof(T) == 1)
inline T __atomic_fetch_and_1(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_FETCH_AND(8, pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 2)
inline T __atomic_fetch_and_2(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_FETCH_AND(16, pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 4)
inline T __atomic_fetch_and_4(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_FETCH_AND_32(pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 8)
inline T __atomic_fetch_and_8(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_FETCH_AND(64, pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
inline T __atomic_fetch_and(T volatile* pointer, T value, int32 memorder)
{
    T result;
    T* resultPtr = memory::AddressOf(result);
    T* valuePtr = memory::AddressOf(value);

    constexpr SizeT size = sizeof(T);
    if constexpr (size == 1) {
        ATOMIC_FETCH_AND(8, pointer, valuePtr, resultPtr, memorder);
    } else if constexpr (size == 2) {
        ATOMIC_FETCH_AND(16, pointer, valuePtr, resultPtr, memorder);
    } else if constexpr (size == 4) {
        ATOMIC_FETCH_AND_32(pointer, valuePtr, resultPtr, memorder);
    } else if constexpr (size == 8) {
        ATOMIC_FETCH_AND(64, pointer, valuePtr, resultPtr, memorder);
    } else {
        NEVER_CALLED("invalid bitwise operation", T);
    }

    return result;
}

export template <typename T>
    requires(sizeof(T) == 1)
inline T __atomic_fetch_or_1(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_FETCH_OR(8, pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 2)
inline T __atomic_fetch_or_2(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_FETCH_OR(16, pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 4)
inline T __atomic_fetch_or_4(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_FETCH_OR_32(pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 8)
inline T __atomic_fetch_or_8(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_FETCH_OR(64, pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
inline T __atomic_fetch_or(T volatile* pointer, T value, int32 memorder)
{
    T result;
    T* resultPtr = memory::AddressOf(result);
    T* valuePtr = memory::AddressOf(value);

    constexpr SizeT size = sizeof(T);
    if constexpr (size == 1) {
        ATOMIC_FETCH_OR(8, pointer, valuePtr, resultPtr, memorder);
    } else if constexpr (size == 2) {
        ATOMIC_FETCH_OR(16, pointer, valuePtr, resultPtr, memorder);
    } else if constexpr (size == 4) {
        ATOMIC_FETCH_OR_32(pointer, valuePtr, resultPtr, memorder);
    } else if constexpr (size == 8) {
        ATOMIC_FETCH_OR(64, pointer, valuePtr, resultPtr, memorder);
    } else {
        NEVER_CALLED("invalid bitwise operation", T);
    }

    return result;
}

export template <typename T>
    requires(sizeof(T) == 1)
inline T __atomic_fetch_xor_1(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_FETCH_XOR(8, pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 2)
inline T __atomic_fetch_xor_2(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_FETCH_XOR(16, pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 4)
inline T __atomic_fetch_xor_4(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_FETCH_XOR_32(pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
    requires(sizeof(T) == 8)
inline T __atomic_fetch_xor_8(T volatile* pointer, T value, int32 memorder)
{
    T result;
    ATOMIC_FETCH_XOR(64, pointer, memory::AddressOf(value), memory::AddressOf(result), memorder);
    return result;
}

export template <typename T>
inline T __atomic_fetch_xor(T volatile* pointer, T value, int32 memorder)
{
    T result;
    T* resultPtr = memory::AddressOf(result);
    T* valuePtr = memory::AddressOf(value);

    constexpr SizeT size = sizeof(T);
    if constexpr (size == 1) {
        ATOMIC_FETCH_XOR(8, pointer, valuePtr, resultPtr, memorder);
    } else if constexpr (size == 2) {
        ATOMIC_FETCH_XOR(16, pointer, valuePtr, resultPtr, memorder);
    } else if constexpr (size == 4) {
        ATOMIC_FETCH_XOR_32(pointer, valuePtr, resultPtr, memorder);
    } else if constexpr (size == 8) {
        ATOMIC_FETCH_XOR(64, pointer, valuePtr, resultPtr, memorder);
    } else {
        NEVER_CALLED("invalid bitwise operation", T);
    }

    return result;
}

export inline CORE_API void __atomic_signal_fence(int32 memorder)
{
    if (memorder == __ATOMIC_RELAXED) {
        return;
    }

    SIGNAL_FENCE();
}

export inline CORE_API void __atomic_thread_fence(int32 memorder)
{
    if (memorder == __ATOMIC_RELAXED) {
        return;
    }

    THREAD_FENCE(memorder);
}

export inline constexpr bool __atomic_always_lock_free(SizeT size, void const volatile*)
{
    if (size > __ATOMIC_MAX_SUPPORT_SIZE) {
        return false;
    }

    SizeT align = __ATOMIC_MAX_SUPPORT_SIZE;
    while (align >= size && align > 0) {
        if (align == size) {
            return true;
        }

        align >>= 1;
    }

    return false;
}

export inline bool __atomic_is_lock_free(SizeT size, void const volatile*)
{
    return __atomic_always_lock_free(size, nullptr);
}