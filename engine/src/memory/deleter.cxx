module;

#include "core/define.h"
#include "core/assert.h"

export module mini.memory:deleter;

import mini.core;

export namespace mini
{

template <typename T, typename Value>
concept DeleterT = NoThrowCopyableT<T> &&
    requires(T del, Value * ptr)
{
    del(ptr);
};

template <NonArrT T>
struct DefaultDeleter
{
    inline constexpr void operator()(T* ptr)
    {
        delete ptr;
    }
};

struct DummyDeleter
{
    template <typename T>
    constexpr void operator()(T&&)
    {
        NEVER_CALLED("dummy deleter should not be called");
    }
};

} // namespace mini