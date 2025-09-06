export module mini.core:static_buffer;

import :type;
import :memory;

namespace mini {

template <UnsignedT T, SizeT CapacityN>
consteval auto IsSizeLimited()
{
    return CapacityN > static_cast<SizeT>(static_cast<T>(-1));
}

template <SizeT CapacityN>
consteval auto SizeTypeSelector() -> decltype(auto)
{
    if constexpr (IsSizeLimited<uint64, CapacityN>())
        return void(0);
    else if constexpr (IsSizeLimited<uint32, CapacityN>())
        return uint64(0);
    else if constexpr (IsSizeLimited<uint16, CapacityN>())
        return uint32(0);
    else if constexpr (IsSizeLimited<uint8, CapacityN>())
        return uint16(0);
    else
        return uint8(0);
}

export template <typename T, SizeT CapacityN, SizeT AlignN = alignof(T)>
class StaticBuffer {
protected:
    alignas(AlignN) byte m_Buffer[sizeof(T) * CapacityN];

public:
    constexpr StaticBuffer() = default;
    constexpr ~StaticBuffer() = default;

    inline T* Data() noexcept { return Address(); }
    inline T const* Data() const noexcept { return Address(); }

    inline constexpr SizeT Alignment() const noexcept { return AlignN; }
    inline constexpr SizeT Capacity() const noexcept { return CapacityN; }

private:
    inline T* Address() const noexcept
    {
        return reinterpret_cast<T*>(const_cast<byte*>(&m_Buffer[0]));
    }

    StaticBuffer(StaticBuffer const&) = delete;
    StaticBuffer(StaticBuffer&&) = delete;
    StaticBuffer& operator=(StaticBuffer const&) = delete;
    StaticBuffer& operator=(StaticBuffer&&) = delete;
};

export template <TrivialT T, SizeT CapacityN, SizeT AlignN>
class StaticBuffer<T, CapacityN, AlignN> {
protected:
    alignas(AlignN) T m_Buffer[CapacityN];

public:
    inline constexpr T* Data() noexcept { return const_cast<T*>(Address()); }
    inline constexpr T const* Data() const noexcept { return Address(); }

    inline constexpr SizeT Alignment() const noexcept { return AlignN; }
    inline constexpr SizeT Capacity() const noexcept { return CapacityN; }

private:
    inline constexpr T const* Address() const noexcept { return &m_Buffer[0]; }
};

export template <SizeT CapacityN>
struct StaticSize {
public:
    typedef decltype(SizeTypeSelector<CapacityN>()) SizeType;

public:
    SizeType size;

    inline constexpr StaticSize() noexcept
        : size(0)
    {
    }

    inline constexpr StaticSize(SizeT s) noexcept
        : size(static_cast<SizeType>(s))
    {
    }

    inline constexpr SizeT Get() const noexcept { return static_cast<SizeT>(size); }
    inline constexpr void Set(SizeT s) noexcept { size = static_cast<SizeType>(s); }
    inline constexpr operator SizeT() const noexcept { return static_cast<SizeT>(size); }

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

export template <typename T, SizeT CapT, SizeT AlignT, typename U, SizeT CapU, SizeT AlignU>
inline constexpr bool operator==(StaticBuffer<T, CapT, AlignT> const& l,
                                 StaticBuffer<U, CapU, AlignU> const& r) noexcept
{
    return l.Data() == r.Data();
}

export template <IntT T, SizeT CapacityN>
inline constexpr auto operator<=>(StaticSize<CapacityN> const& s, T o) noexcept
{
    return static_cast<T>(s.size) <=> o;
}

export template <SizeT LCapN, SizeT RCapN>
inline constexpr auto operator<=>(StaticSize<LCapN> const& l, StaticSize<RCapN> const& r) noexcept
{
    using CommonSizeT = CommonT<decltype(l.size), decltype(r.size)>;
    return static_cast<CommonSizeT>(l.size) <=> static_cast<CommonSizeT>(r.size);
}

} // namespace mini