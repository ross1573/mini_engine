#pragma once

#include "math/common.h"

namespace mini::detail
{

template <typename T, SizeT CapacityN>
consteval auto IsSizeLimited()
{
    return CapacityN > uint64(std::numeric_limits<T>::max() - 1);
}

template <SizeT CapacityN>
consteval auto SizeTypeSelector() -> decltype(auto)
{
    if constexpr (IsSizeLimited<uint16, CapacityN>()) return uint32(0);
    else if constexpr (IsSizeLimited<uint8, CapacityN>()) return uint16(0);
    else return uint8(0);
}

template <SizeT CapacityN>
using SizeType = decltype(SizeTypeSelector<CapacityN>());

} // namespace mini::detail

namespace mini
{

template <
    typename T,
    SizeT CapacityN,
    SizeT AlignN = alignof(T)
>
class StaticBuffer
{
protected:
    alignas(AlignN) byte m_Buffer[sizeof(T) * CapacityN];

public:
    constexpr StaticBuffer() noexcept = default;
    constexpr ~StaticBuffer() noexcept = default;
    StaticBuffer(StaticBuffer const&) = delete;
    StaticBuffer(StaticBuffer&&) = delete;

    [[force_inline]] T* Data() noexcept
    {
        return Address();
    }

    [[force_inline]] T const* Data() const noexcept
    {
        return Address();
    }

    [[force_inline]] constexpr SizeT Alignment() const noexcept { return AlignN; }
    [[force_inline]] constexpr SizeT Capacity() const noexcept { return CapacityN; }

    [[force_inline]] bool operator==(StaticBuffer const& other) const noexcept
    {
        return Address() == other.Address();
    }

private:
    [[force_inline]] T* Address() const noexcept
    {
        return reinterpret_cast<T*>(const_cast<byte*>(&m_Buffer[0]));
    }

    StaticBuffer& operator=(StaticBuffer const&) = delete;
    StaticBuffer& operator=(StaticBuffer&&) = delete;
};

template <
    TrivialT T,
    SizeT CapacityN,
    SizeT AlignN
>
class StaticBuffer<T, CapacityN, AlignN>
{
protected:
    alignas(AlignN) T m_Buffer[CapacityN];

public:
    constexpr StaticBuffer() noexcept = default;
    constexpr ~StaticBuffer() noexcept = default;
    StaticBuffer(StaticBuffer const&) = delete;
    StaticBuffer(StaticBuffer&&) = delete;

    [[force_inline]] constexpr T* Data() noexcept
    {
        return const_cast<T*>(Address());
    }

    [[force_inline]] constexpr T const* Data() const noexcept
    {
        return Address();
    }

    [[force_inline]] constexpr SizeT Alignment() const noexcept { return AlignN; }
    [[force_inline]] constexpr SizeT Capacity() const noexcept { return CapacityN; }

    [[force_inline]] constexpr bool operator==(StaticBuffer const& other) const noexcept
    {
        return Address() == other.Address();
    }

private:
    [[force_inline]] constexpr T const* Address() const noexcept
    {
        return &m_Buffer[0];
    }

    StaticBuffer& operator=(StaticBuffer const&) = delete;
    StaticBuffer& operator=(StaticBuffer&&) = delete;
};

template <SizeT CapacityN>
struct StaticSize
{
    typedef detail::SizeType<CapacityN> SizeType;

    SizeType size;

    constexpr StaticSize() noexcept = default;
    [[force_inline]] constexpr StaticSize(SizeT s) noexcept
        : size(static_cast<SizeType>(s))
    {
    }

    [[force_inline]] constexpr SizeT Get() const noexcept
    {
        return static_cast<SizeT>(size);
    }

    [[force_inline]] constexpr void Set(SizeT s) noexcept
    {
        size = static_cast<SizeType>(s);
    }

    [[force_inline]] constexpr operator SizeT() const noexcept
    {
        return static_cast<SizeT>(size);
    }

    [[force_inline]] constexpr StaticSize& operator++() noexcept
    {
        ++size;
        return *this;
    }

    [[force_inline]] constexpr StaticSize& operator--() noexcept
    {
        --size;
        return *this;
    }

    [[force_inline]] constexpr StaticSize& operator+=(IntT auto s) noexcept
    {
        size += static_cast<SizeType>(s);
        return *this;
    }

    [[force_inline]] constexpr StaticSize& operator-=(IntT auto s) noexcept
    {
        size -= static_cast<SizeType>(s);
        return *this;
    }

    [[force_inline]] constexpr StaticSize operator++(int) noexcept
    {
        StaticSize s = *this;
        ++size;
        return s;
    }

    [[force_inline]] constexpr StaticSize operator--(int) noexcept
    {
        StaticSize s = *this;
        --size;
        return s;
    }

    [[force_inline]] constexpr StaticSize& operator=(SizeT s) noexcept
    {
        size = static_cast<SizeType>(s);
        return *this;
    }
};

template <IntT T, SizeT CapacityN>
[[force_inline]] constexpr auto operator<=>(StaticSize<CapacityN> const& s, T o) noexcept
{
    return static_cast<T>(s.size) <=> o;
}

template <SizeT LCapN, SizeT RCapN>
[[force_inline]] constexpr auto operator<=>(StaticSize<LCapN> const& l,
                                            StaticSize<RCapN> const& r) noexcept
{
    using CommonSizeT = std::common_type_t<decltype(l.size), decltype(r.size)>;
    return static_cast<CommonSizeT>(l.size) <=> static_cast<CommonSizeT>(r.size);
}

} // namespace mini