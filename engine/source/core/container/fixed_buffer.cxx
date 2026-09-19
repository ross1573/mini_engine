module;
#include "debug/assert.h"
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

    [[nodiscard]] T* Data() noexcept { return Address(); }
    [[nodiscard]] T const* Data() const noexcept { return Address(); }

    [[nodiscard]] constexpr size_t Alignment() const noexcept { return AlignN; }
    [[nodiscard]] constexpr size_t Capacity() const noexcept { return CapacityN; }

private:
    T* Address() const noexcept { return reinterpret_cast<T*>(const_cast<byte*>(&m_buffer[0])); }

public:
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

    [[nodiscard]] constexpr T* Data() noexcept { return const_cast<T*>(Address()); }
    [[nodiscard]] constexpr T const* Data() const noexcept { return Address(); }

    [[nodiscard]] constexpr size_t Alignment() const noexcept { return AlignN; }
    [[nodiscard]] constexpr size_t Capacity() const noexcept { return CapacityN; }

private:
    constexpr T const* Address() const noexcept { return &m_buffer[0]; }
};

template <UnsignedT T, size_t CapacityN>
consteval auto IsSizeLimited()
{
    return CapacityN > static_cast<size_t>(static_cast<T>(-1));
}

template <size_t CapacityN>
consteval auto SizeTypeSelector() -> decltype(auto)
{
    if constexpr (IsSizeLimited<uint64, CapacityN>()) {
        UNSUPPORTED("invalid capacity", decltype(CapacityN));
    } else if constexpr (IsSizeLimited<uint32, CapacityN>()) {
        return static_cast<uint64>(0);
    } else if constexpr (IsSizeLimited<uint16, CapacityN>()) {
        return static_cast<uint32>(0);
    } else if constexpr (IsSizeLimited<uint8, CapacityN>()) {
        return static_cast<uint16>(0);
    } else {
        return static_cast<uint8>(0);
    }
}

template <size_t CapacityN>
struct FixedSize {
public:
    typedef decltype(SizeTypeSelector<CapacityN>()) SizeType;

public:
    SizeType size;

    constexpr FixedSize() noexcept = default;

    constexpr FixedSize(size_t size) noexcept
        : size(static_cast<SizeType>(size))
    {
    }

    [[nodiscard]] constexpr size_t Get() const noexcept { return static_cast<size_t>(size); }
    constexpr void Set(size_t newSize) noexcept { size = static_cast<SizeType>(newSize); }
    constexpr operator size_t() const noexcept { return static_cast<size_t>(size); }

    constexpr FixedSize& operator++() noexcept
    {
        ++size;
        return *this;
    }

    constexpr FixedSize& operator--() noexcept
    {
        --size;
        return *this;
    }

    constexpr FixedSize& operator+=(IntegralT auto sizeValue) noexcept
    {
        size += static_cast<SizeType>(sizeValue);
        return *this;
    }

    constexpr FixedSize& operator-=(IntegralT auto sizeValue) noexcept
    {
        size -= static_cast<SizeType>(sizeValue);
        return *this;
    }

    constexpr FixedSize operator++(int32) noexcept
    {
        FixedSize s = *this;
        ++size;
        return s;
    }

    constexpr FixedSize operator--(int32) noexcept
    {
        FixedSize s = *this;
        --size;
        return s;
    }

    constexpr FixedSize& operator=(size_t newSize) noexcept
    {
        size = static_cast<SizeType>(newSize);
        return *this;
    }
};

template <typename T, size_t CapT, size_t AlignT, typename U, size_t CapU, size_t AlignU>
constexpr bool operator==(FixedBuffer<T, CapT, AlignT> const& lhs, FixedBuffer<U, CapU, AlignU> const& rhs) noexcept
{
    return lhs.Data() == rhs.Data();
}

template <IntegralT T, size_t CapacityN>
constexpr auto operator<=>(FixedSize<CapacityN> const& lhs, T rhs) noexcept
{
    return static_cast<T>(lhs.size) <=> rhs;
}

template <size_t LCapN, size_t RCapN>
constexpr auto operator<=>(FixedSize<LCapN> const& lhs, FixedSize<RCapN> const& rhs) noexcept
{
    using CommonSizeT = CommonT<decltype(lhs.size), decltype(rhs.size)>;
    return static_cast<CommonSizeT>(lhs.size) <=> static_cast<CommonSizeT>(rhs.size);
}

} // namespace mini::memory