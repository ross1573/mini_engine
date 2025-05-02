export module mini.core:iterator_version;

import :type;

export namespace mini {

template <typename ContainerT>
concept StaticContainerT = requires(ContainerT c) {
    c.IsFull(); // TODO: check static container with more explicit methods
};

template <typename ContainerT>
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

template <StaticContainerT ContainerT>
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

template <typename T, typename U>
inline constexpr bool operator==(IteratorVersion<T> const& l, IteratorVersion<U> const& r) noexcept
{
    return l.version == r.version;
}

template <typename T>
inline constexpr bool operator==(IteratorVersion<T> const& l, SizeT r) noexcept
{
    return l.version == r;
}

template <typename T, StaticContainerT U>
inline constexpr bool operator==(IteratorVersion<T> const& l, IteratorVersion<U> const& r) noexcept
    requires EqualityComparableWithT<T, U>
{
    return true;
}

template <StaticContainerT T, StaticContainerT U>
inline constexpr bool operator==(IteratorVersion<T> const& l, IteratorVersion<U> const& r) noexcept
    requires EqualityComparableWithT<T, U>
{
    return true;
}

} // namespace mini