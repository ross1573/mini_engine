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

    [[nodiscard]] T* data() noexcept { return address(); }
    [[nodiscard]] T const* data() const noexcept { return address(); }

    [[nodiscard]] constexpr size_t alignment() const noexcept { return AlignN; }
    [[nodiscard]] constexpr size_t capacity() const noexcept { return CapacityN; }

private:
    T* address() const noexcept { return reinterpret_cast<T*>(const_cast<byte*>(&m_buffer[0])); }

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

    [[nodiscard]] constexpr T* data() noexcept { return const_cast<T*>(address()); }
    [[nodiscard]] constexpr T const* data() const noexcept { return address(); }

    [[nodiscard]] constexpr size_t alignment() const noexcept { return AlignN; }
    [[nodiscard]] constexpr size_t capacity() const noexcept { return CapacityN; }

private:
    constexpr T const* address() const noexcept { return &m_buffer[0]; }
};

template <UnsignedT T, size_t CapacityN>
consteval auto isSizeLimited()
{
    return CapacityN > static_cast<size_t>(static_cast<T>(-1));
}

template <size_t CapacityN>
consteval auto sizeTypeSelector() -> decltype(auto)
{
    if constexpr (isSizeLimited<uint64, CapacityN>()) {
        UNSUPPORTED("invalid capacity", decltype(CapacityN));
    } else if constexpr (isSizeLimited<uint32, CapacityN>()) {
        return static_cast<uint64>(0);
    } else if constexpr (isSizeLimited<uint16, CapacityN>()) {
        return static_cast<uint32>(0);
    } else if constexpr (isSizeLimited<uint8, CapacityN>()) {
        return static_cast<uint16>(0);
    } else {
        return static_cast<uint8>(0);
    }
}

template <size_t CapacityN>
struct FixedSize {
public:
    typedef decltype(sizeTypeSelector<CapacityN>()) SizeType;

public:
    SizeType size;

    constexpr FixedSize() noexcept = default;

    constexpr FixedSize(size_t size) noexcept
        : size(static_cast<SizeType>(size))
    {
    }

    [[nodiscard]] constexpr size_t get() const noexcept { return static_cast<size_t>(size); }
    constexpr void set(size_t newSize) noexcept { size = static_cast<SizeType>(newSize); }
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
    return lhs.data() == rhs.data();
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