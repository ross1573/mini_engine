export module mini.core:fixed_buffer;

import :type;
import :numeric;

namespace mini::memory {

template <typename T, size_t CapacityN, size_t AlignN = alignof(T)>
class FixedBuffer {
protected:
    alignas(AlignN) byte m_buffer[sizeof(T) * CapacityN];

public:
    constexpr FixedBuffer() = default;
    constexpr ~FixedBuffer() = default;

    inline T* Data() noexcept { return Address(); }
    inline T const* Data() const noexcept { return Address(); }

    inline constexpr size_t Alignment() const noexcept { return AlignN; }
    inline constexpr size_t Capacity() const noexcept { return CapacityN; }

private:
    inline T* Address() const noexcept { return reinterpret_cast<T*>(const_cast<byte*>(&m_buffer[0])); }

    FixedBuffer(FixedBuffer const&) = delete;
    FixedBuffer(FixedBuffer&&) = delete;
    FixedBuffer& operator=(FixedBuffer const&) = delete;
    FixedBuffer& operator=(FixedBuffer&&) = delete;
};

template <TrivialT T, size_t CapacityN, size_t AlignN>
class FixedBuffer<T, CapacityN, AlignN> {
protected:
    alignas(AlignN) T m_buffer[CapacityN];

public:
    constexpr FixedBuffer() = default;
    constexpr ~FixedBuffer() = default;

    inline constexpr T* Data() noexcept { return const_cast<T*>(Address()); }
    inline constexpr T const* Data() const noexcept { return Address(); }

    inline constexpr size_t Alignment() const noexcept { return AlignN; }
    inline constexpr size_t Capacity() const noexcept { return CapacityN; }

private:
    inline constexpr T const* Address() const noexcept { return &m_buffer[0]; }
};

template <UnsignedT T, size_t CapacityN>
consteval auto IsSizeLimited()
{
    return CapacityN > static_cast<size_t>(static_cast<T>(-1));
}

template <size_t CapacityN>
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

template <size_t CapacityN>
struct FixedSize {
public:
    typedef decltype(SizeTypeSelector<CapacityN>()) SizeType;

public:
    SizeType size;

    constexpr FixedSize() noexcept = default;

    inline constexpr FixedSize(size_t s) noexcept
        : size(static_cast<SizeType>(s))
    {
    }

    inline constexpr size_t Get() const noexcept { return static_cast<size_t>(size); }
    inline constexpr void Set(size_t s) noexcept { size = static_cast<SizeType>(s); }
    inline constexpr operator size_t() const noexcept { return static_cast<size_t>(size); }

    inline constexpr FixedSize& operator++() noexcept
    {
        ++size;
        return *this;
    }

    inline constexpr FixedSize& operator--() noexcept
    {
        --size;
        return *this;
    }

    inline constexpr FixedSize& operator+=(IntegralT auto s) noexcept
    {
        size += static_cast<SizeType>(s);
        return *this;
    }

    inline constexpr FixedSize& operator-=(IntegralT auto s) noexcept
    {
        size -= static_cast<SizeType>(s);
        return *this;
    }

    inline constexpr FixedSize operator++(int32) noexcept
    {
        FixedSize s = *this;
        ++size;
        return s;
    }

    inline constexpr FixedSize operator--(int32) noexcept
    {
        FixedSize s = *this;
        --size;
        return s;
    }

    inline constexpr FixedSize& operator=(size_t s) noexcept
    {
        size = static_cast<SizeType>(s);
        return *this;
    }
};

template <typename T, size_t CapT, size_t AlignT, typename U, size_t CapU, size_t AlignU>
inline constexpr bool operator==(FixedBuffer<T, CapT, AlignT> const& l, FixedBuffer<U, CapU, AlignU> const& r) noexcept
{
    return l.Data() == r.Data();
}

template <IntegralT T, size_t CapacityN>
inline constexpr auto operator<=>(FixedSize<CapacityN> const& s, T o) noexcept
{
    return static_cast<T>(s.size) <=> o;
}

template <size_t LCapN, size_t RCapN>
inline constexpr auto operator<=>(FixedSize<LCapN> const& l, FixedSize<RCapN> const& r) noexcept
{
    using CommonSizeT = CommonT<decltype(l.size), decltype(r.size)>;
    return static_cast<CommonSizeT>(l.size) <=> static_cast<CommonSizeT>(r.size);
}

} // namespace mini::memory