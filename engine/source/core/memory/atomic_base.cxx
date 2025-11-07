export module mini.core:atomic_base;

#if ARCH_ARM64
#  if PLATFORM_MACOS
#    define ATOMIC_INTERFERENCE_SIZE 128
#    define ATOMIC_SUPPORTED_SIZE    16
#  else
#    define ATOMIC_INTERFERENCE_SIZE 64
#    define ATOMIC_SUPPORTED_SIZE    16
#  endif
#elif ARCH_X86_64
#  define ATOMIC_INTERFERENCE_SIZE 64
#  define ATOMIC_SUPPORTED_SIZE    16
#elif ARCH_X86
#  define ATOMIC_INTERFERENCE_SIZE 32
#  define ATOMIC_SUPPORTED_SIZE    8
#else
#  error "unsupported architecture"
#endif

import :type;

namespace mini::memory {

constexpr SizeT AtomicInterferenceSize = ATOMIC_INTERFERENCE_SIZE;
constexpr SizeT AtomicSupportedMaxSize = ATOMIC_SUPPORTED_SIZE;

template <typename T>
consteval bool IsAtomicSupported()
{
    if (sizeof(T) > AtomicSupportedMaxSize) {
        return false;
    }

    SizeT size = AtomicSupportedMaxSize;
    while (size > 0) {
        if (size == sizeof(T)) {
            return true;
        }

        size >>= 1;
    }

    return false;
}

template <typename T>
consteval SizeT AtomicAlignment()
{
    if (sizeof(T) > AtomicSupportedMaxSize) {
        return alignof(T);
    }

    SizeT align = AtomicSupportedMaxSize;
    while (align > 0) {
        if ((align >> 1) < sizeof(T)) {
            return align;
        }

        align >>= 1;
    }

    return 1;
}

template <typename T, SizeT TableN>
inline SizeT AtomicAddressHash(T const volatile* loc) noexcept
{
    SizeT intptr = reinterpret_cast<SizeT>(loc);
    SizeT hash = intptr >> 6; // discard lower bits
    hash ^= hash >> 16;       // xor with higher bits
    hash &= (TableN - 1);     // get bucket
    return hash;
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

} // namespace mini::memory