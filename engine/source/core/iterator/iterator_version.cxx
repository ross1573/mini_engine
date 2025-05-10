export module mini.core:iterator_version;

import :type;

namespace mini {

template <typename ContainerT>
concept StaticContainerT = requires(ContainerT c) {
    c.IsFull(); // TODO: check static container with more explicit methods
};

export template <typename ContainerT>
struct IteratorVersion {
public:
    SizeT version;

    inline constexpr IteratorVersion(SizeT v) noexcept
        : version(v)
    {
    }

    template <ConvertibleToT<ContainerT> ContainerU>
    inline constexpr IteratorVersion(IteratorVersion<ContainerU> const& o) noexcept
        : version(o.version)
    {
    }

    inline constexpr operator SizeT() noexcept { return version; }

    inline constexpr IteratorVersion& operator=(SizeT v) noexcept
    {
        version = v;
        return *this;
    }
};

export template <StaticContainerT ContainerT>
struct IteratorVersion<ContainerT> {
public:
    inline constexpr IteratorVersion(SizeT) noexcept {}

    template <ConvertibleToT<ContainerT> ContainerU>
    inline constexpr IteratorVersion(IteratorVersion<ContainerU> const&) noexcept
    {
    }

    inline constexpr operator SizeT() noexcept { return 0; }

    inline constexpr IteratorVersion& operator=(SizeT) noexcept { return *this; }
};

export template <typename T, typename U>
inline constexpr bool operator==(IteratorVersion<T> const& l, IteratorVersion<U> const& r) noexcept
{
    return l.version == r.version;
}

export template <typename T>
inline constexpr bool operator==(IteratorVersion<T> const& l, SizeT r) noexcept
{
    return l.version == r;
}

export template <typename T, StaticContainerT U>
inline constexpr bool operator==(IteratorVersion<T> const&, IteratorVersion<U> const&) noexcept
    requires EqualityComparableWithT<T, U>
{
    return true;
}

export template <StaticContainerT T, StaticContainerT U>
inline constexpr bool operator==(IteratorVersion<T> const&, IteratorVersion<U> const&) noexcept
    requires EqualityComparableWithT<T, U>
{
    return true;
}

} // namespace mini