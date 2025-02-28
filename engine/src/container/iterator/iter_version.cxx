module;

#include "core/define.h"

export module mini.iterator:iter_version;

import mini.core;

export namespace mini
{

template <typename ContainerT>
concept StaticContainerT = requires(ContainerT c)
{
    c.IsFull(); // TODO: check static container with more explicit methods
};

template <typename ContainerT>
struct IteratorVersion
{
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

    inline constexpr operator SizeT() noexcept
    {
        return version;
    }

    inline constexpr IteratorVersion& operator=(SizeT v) noexcept
    {
        version = v;
        return *this;
    }

    template <EqualityComparableWithT<ContainerT> ContainerU>
    inline constexpr bool operator==(IteratorVersion<ContainerU> const& o) const noexcept
    {
        return version == o.version;
    }

    inline constexpr bool operator==(SizeT v) const noexcept
    {
        return version == v;
    }
};

template <StaticContainerT ContainerT>
struct IteratorVersion<ContainerT>
{
    inline constexpr IteratorVersion(SizeT) noexcept
    {
    }

    template <ConvertibleToT<ContainerT> ContainerU>
    inline constexpr IteratorVersion(IteratorVersion<ContainerU> const&) noexcept
    {
    }

    inline constexpr operator SizeT() noexcept
    {
        return 0;
    }

    inline constexpr IteratorVersion& operator=(SizeT) noexcept
    {
        return *this;
    }

    template <EqualityComparableWithT<ContainerT> ContainerU>
    inline constexpr bool operator==(IteratorVersion<ContainerU> const&) const noexcept
    {
        return true;
    }

    inline constexpr bool operator==(SizeT) const noexcept
    {
        return true;
    }
};

} // namespace mini