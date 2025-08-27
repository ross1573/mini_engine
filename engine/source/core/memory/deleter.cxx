export module mini.core:deleter;

import :type;

namespace mini {

export template <typename T, typename Value>
concept DeleterT = NoThrowCopyableT<T> && requires(T del, Value* ptr) { del(ptr); };

export template <NonArrT T>
struct DefaultDeleter {
public:
    inline constexpr void operator()(T* ptr) { delete ptr; }
};

export struct DummyDeleter {
public:
    template <typename T>
    constexpr void operator()(T&&)
    {
        NEVER_CALLED("dummy deleter should not be called");
    }
};

} // namespace mini