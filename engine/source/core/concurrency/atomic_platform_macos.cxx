export module mini.core:atomic_platform;

#if ARCH_ARM64
#  define ATOMIC_INTERFERENCE_SIZE 128
#  define ATOMIC_SUPPORTED_SIZE    16
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

export constexpr mini::int32 __ATOMIC_INTERFERENCE_SIZE = ATOMIC_INTERFERENCE_SIZE;
export constexpr mini::int32 __ATOMIC_MAX_SUPPORT_SIZE = ATOMIC_SUPPORTED_SIZE;