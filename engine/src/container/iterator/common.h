#pragma once

#include "core/utility.h"

namespace mini
{

template <typename T>
concept ForwardIteratorT = CopyableT<T> && EqualityComparableT<T> &&
    requires(T i, T const j, SizeT const n)
{
    requires !RefT<typename T::Value>;
    requires PtrT<typename T::Ptr>;
    requires RefT<typename T::Ref>;

    { i.IsValid() } -> ConvertibleToT<bool>;
    { i.IsValidWith(j) } -> ConvertibleToT<bool>;
    { i.Increment() } -> ConvertibleToT<bool>;

    { i.Address() } -> SameAsT<typename T::Ptr>;
    { i.operator->() } -> SameAsT<typename T::Ptr>;
    { *i } -> SameAsT<typename T::Ref>;
    { ++i } -> SameAsT<T&>;
    { i++ } -> SameAsT<T>;
};

template <typename T>
concept BidrectionalIteratorT = ForwardIteratorT<T> &&
    requires(T i)
{
    { i.Decrement() } -> ConvertibleToT<bool>;

    { --i } -> SameAsT<T&>;
    { i-- } -> SameAsT<T>;
};

template <typename T>
concept RandomAccessIteratorT = BidrectionalIteratorT<T> && ComparableT<T> &&
    requires(T i, T const j, OffsetT n, SizeT s)
{
    { i.Advance(n) } -> ConvertibleToT<bool>;

    { i += n } -> SameAsT<T&>;
    { i -= n } -> SameAsT<T&>;
    { j + n } -> SameAsT<T>;
    { j - n } -> SameAsT<T>;
    { n + j } -> SameAsT<T>;
    { j - i } -> SameAsT<OffsetT>;
    { j[n] } -> SameAsT<typename T::Ref>;
};

template <typename T, typename U>
concept IteratorCopyableFromT = ForwardIteratorT<T> && ForwardIteratorT<U> &&
    requires(T i, U v)
{
    *i = ForwardArg<typename U::Value>(*v);
};

template <typename T, typename U>
concept IteratorMovableFromT = ForwardIteratorT<T> && ForwardIteratorT<U> &&
    requires(T i, U v)
{
    *i = MoveArg(*v);
};

template <typename Iter, typename T>
concept ForwardIteratableByT = (ForwardIteratorT<Iter> &&
                                ConvertibleToT<typename Iter::Value, T>);

template <typename Iter, typename T>
concept BidirectionalIteratableByT = (BidrectionalIteratorT<Iter> &&
                                      ConvertibleToT<typename Iter::Value, T>);

template <typename Iter, typename T>
concept RandomAccessIteratableByT = (RandomAccessIteratorT<Iter> &&
                                     ConvertibleToT<typename Iter::Value, T>);

template <typename T>
[[force_inline]] constexpr typename T::Iterator begin(T& c)
    requires ForwardIteratorT<typename T::Iterator>
{
    return c.Begin();
}

template <typename T>
[[force_inline]] constexpr typename T::Iterator end(T& c)
    requires ForwardIteratorT<typename T::Iterator>
{
    return c.End();
}

template <typename T>
[[force_inline]] constexpr typename T::ConstIterator cbegin(T const& c)
    requires ForwardIteratorT<typename T::ConstIterator>
{
    return c.Begin();
}

template <typename T>
[[force_inline]] constexpr typename T::ConstIterator cend(T const& c)
    requires ForwardIteratorT<typename T::ConstIterator>
{
    return c.End();
}

} // namespace mini

namespace mini::detail
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

    [[force_inline]] constexpr IteratorVersion(SizeT v) noexcept
        : version(v)
    {
    }

    template <ConvertibleToT<ContainerT> ContainerU>
    [[force_inline]] constexpr IteratorVersion(IteratorVersion<ContainerU> const& o) noexcept
        : version(o.version)
    {
    }

    [[force_inline]] constexpr operator SizeT() noexcept
    {
        return version;
    }

    [[force_inline]] constexpr IteratorVersion& operator=(SizeT v) noexcept
    {
        version = v;
        return *this;
    }

    template <EqualityComparableWithT<ContainerT> ContainerU>
    [[force_inline]] constexpr bool operator==(IteratorVersion<ContainerU> const& o) const noexcept
    {
        return version == o.version;
    }

    [[force_inline]] constexpr bool operator==(SizeT v) const noexcept
    {
        return version == v;
    }
};

template <StaticContainerT ContainerT>
struct IteratorVersion<ContainerT>
{
    [[force_inline]] constexpr IteratorVersion(SizeT) noexcept
    {
    }

    template <ConvertibleToT<ContainerT> ContainerU>
    [[force_inline]] constexpr IteratorVersion(IteratorVersion<ContainerU> const&) noexcept
    {
    }

    [[force_inline]] constexpr operator SizeT() noexcept
    {
        return 0;
    }

    [[force_inline]] constexpr IteratorVersion& operator=(SizeT) noexcept
    {
        return *this;
    }

    template <EqualityComparableWithT<ContainerT> ContainerU>
    [[force_inline]] constexpr bool operator==(IteratorVersion<ContainerU> const&) const noexcept
    {
        return true;
    }

    [[force_inline]] constexpr bool operator==(SizeT) const noexcept
    {
        return true;
    }
};

} // namespace mini::detail

namespace std
{

template <mini::ForwardIteratorT T>
struct iterator_traits<T>
{
    typedef std::forward_iterator_tag iterator_category;
    typedef typename T::Value value_type;
    typedef typename T::Ptr pointer;
    typedef typename T::Ref reference;
    typedef mini::OffsetT difference_type;
};

template <mini::BidrectionalIteratorT T>
struct iterator_traits<T>
{
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef typename T::Value value_type;
    typedef typename T::Ptr pointer;
    typedef typename T::Ref reference;
    typedef mini::OffsetT difference_type;
};

template <mini::RandomAccessIteratorT T>
struct iterator_traits<T>
{
    typedef std::random_access_iterator_tag iterator_category;
    typedef typename T::Value value_type;
    typedef typename T::Ptr pointer;
    typedef typename T::Ref reference;
    typedef mini::OffsetT difference_type;
};

} // namespace std