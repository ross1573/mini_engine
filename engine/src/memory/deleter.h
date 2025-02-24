#pragma once

#include "core/utility.h"

namespace mini
{

template <typename T, typename Value>
concept DeleterT = NoThrowCopyableT<T> &&
    requires(T del, Value * ptr)
{
    del(ptr);
};

template <typename T>
    requires (!std::is_array_v<T>)
struct DefaultDeleter
{
    [[force_inline]] constexpr void operator()(T* ptr)
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