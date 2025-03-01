module;

#include <limits>
#include "core/define.h"

export module mini.core:static_buffer;

import :type;
import :math;

namespace mini
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

}

export namespace mini
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
    inline constexpr StaticBuffer() noexcept = default;
    inline constexpr ~StaticBuffer() noexcept = default;
    StaticBuffer(StaticBuffer const&) = delete;
    StaticBuffer(StaticBuffer&&) = delete;

    inline T* Data() noexcept
    {
        return Address();
    }

    inline T const* Data() const noexcept
    {
        return Address();
    }

    inline constexpr SizeT Alignment() const noexcept { return AlignN; }
    inline constexpr SizeT Capacity() const noexcept { return CapacityN; }

    inline bool operator==(StaticBuffer const& other) const noexcept
    {
        return Address() == other.Address();
    }

private:
    inline T* Address() const noexcept
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
    inline constexpr StaticBuffer() noexcept = default;
    inline constexpr ~StaticBuffer() noexcept = default;
    StaticBuffer(StaticBuffer const&) = delete;
    StaticBuffer(StaticBuffer&&) = delete;

    inline constexpr T* Data() noexcept
    {
        return const_cast<T*>(Address());
    }

    inline constexpr T const* Data() const noexcept
    {
        return Address();
    }

    inline constexpr SizeT Alignment() const noexcept { return AlignN; }
    inline constexpr SizeT Capacity() const noexcept { return CapacityN; }

    inline constexpr bool operator==(StaticBuffer const& other) const noexcept
    {
        return Address() == other.Address();
    }

private:
    inline constexpr T const* Address() const noexcept
    {
        return &m_Buffer[0];
    }

    StaticBuffer& operator=(StaticBuffer const&) = delete;
    StaticBuffer& operator=(StaticBuffer&&) = delete;
};

template <SizeT CapacityN>
struct StaticSize
{
    typedef SizeType<CapacityN> SizeType;

    SizeType size;

    inline constexpr StaticSize() noexcept = default;
    inline constexpr StaticSize(SizeT s) noexcept
        : size(static_cast<SizeType>(s))
    {
    }

    inline constexpr SizeT Get() const noexcept
    {
        return static_cast<SizeT>(size);
    }

    inline constexpr void Set(SizeT s) noexcept
    {
        size = static_cast<SizeType>(s);
    }

    inline constexpr operator SizeT() const noexcept
    {
        return static_cast<SizeT>(size);
    }

    inline constexpr StaticSize& operator++() noexcept
    {
        ++size;
        return *this;
    }

    inline constexpr StaticSize& operator--() noexcept
    {
        --size;
        return *this;
    }

    inline constexpr StaticSize& operator+=(IntT auto s) noexcept
    {
        size += static_cast<SizeType>(s);
        return *this;
    }

    inline constexpr StaticSize& operator-=(IntT auto s) noexcept
    {
        size -= static_cast<SizeType>(s);
        return *this;
    }

    inline constexpr StaticSize operator++(int) noexcept
    {
        StaticSize s = *this;
        ++size;
        return s;
    }

    inline constexpr StaticSize operator--(int) noexcept
    {
        StaticSize s = *this;
        --size;
        return s;
    }

    inline constexpr StaticSize& operator=(SizeT s) noexcept
    {
        size = static_cast<SizeType>(s);
        return *this;
    }
};

template <IntT T, SizeT CapacityN>
inline constexpr auto operator<=>(StaticSize<CapacityN> const& s, T o) noexcept
{
    return static_cast<T>(s.size) <=> o;
}

template <SizeT LCapN, SizeT RCapN>
inline constexpr auto operator<=>(StaticSize<LCapN> const& l,
                                            StaticSize<RCapN> const& r) noexcept
{
    using CommonSizeT = CommonT<decltype(l.size), decltype(r.size)>;
    return static_cast<CommonSizeT>(l.size) <=> static_cast<CommonSizeT>(r.size);
}

} // namespace mini