module;

#include <iterator>

export module mini.core:iterator;

import :type;
import :utility;

export namespace mini {

template <typename T>
concept ForwardIteratorT = CopyableT<T> && EqualityComparableT<T> && //
                           requires(T i, T const j, SizeT const n)   //
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
    { ++i } -> SameAsT<T &>;
    { i++ } -> SameAsT<T>;
};

template <typename T>
concept BidrectionalIteratorT = ForwardIteratorT<T> && //
                                requires(T i)          //
{
    { i.Decrement() } -> ConvertibleToT<bool>;

    { --i } -> SameAsT<T &>;
    { i-- } -> SameAsT<T>;
};

template <typename T>
concept RandomAccessIteratorT = BidrectionalIteratorT<T> && ComparableT<T> && //
                                requires(T i, T const j, OffsetT n, SizeT s)  //
{
    { i.Advance(n) } -> ConvertibleToT<bool>;

    { i += n } -> SameAsT<T &>;
    { i -= n } -> SameAsT<T &>;
    { j + n } -> SameAsT<T>;
    { j - n } -> SameAsT<T>;
    { n + j } -> SameAsT<T>;
    { j - i } -> SameAsT<OffsetT>;
    { j[n] } -> SameAsT<typename T::Ref>;
};

template <typename T, typename U>
concept IteratorCopyableFromT = ForwardIteratorT<T> && ForwardIteratorT<U> &&
                                requires(T i, U v) { *i = ForwardArg<typename U::Value>(*v); };

template <typename T, typename U>
concept IteratorMovableFromT = ForwardIteratorT<T> && ForwardIteratorT<U> && //
                               requires(T i, U v) { *i = MoveArg(*v); };

template <typename Iter, typename T>
concept ForwardIteratableByT = ForwardIteratorT<Iter> && ConvertibleToT<typename Iter::Value, T>;

template <typename Iter, typename T>
concept BidirectionalIteratableByT = BidrectionalIteratorT<Iter> && ConvertibleToT<typename Iter::Value, T>;

template <typename Iter, typename T>
concept RandomAccessIteratableByT = RandomAccessIteratorT<Iter> && ConvertibleToT<typename Iter::Value, T>;

template <typename T>
inline constexpr typename T::Iterator begin(T &c) requires ForwardIteratorT<typename T::Iterator>
{
    return c.Begin();
}

template <typename T>
inline constexpr typename T::Iterator end(T &c) requires ForwardIteratorT<typename T::Iterator>
{
    return c.End();
}

template <typename T>
inline constexpr typename T::ConstIterator cbegin(T const &c) requires ForwardIteratorT<typename T::ConstIterator>
{
    return c.Begin();
}

template <typename T>
inline constexpr typename T::ConstIterator cend(T const &c) requires ForwardIteratorT<typename T::ConstIterator>
{
    return c.End();
}

} // namespace mini

export namespace std {

template <mini::ForwardIteratorT T>
struct iterator_traits<T> {
    typedef std::forward_iterator_tag iterator_category;
    typedef typename T::Value value_type;
    typedef typename T::Ptr pointer;
    typedef typename T::Ref reference;
    typedef mini::OffsetT difference_type;
};

template <mini::BidrectionalIteratorT T>
struct iterator_traits<T> {
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef typename T::Value value_type;
    typedef typename T::Ptr pointer;
    typedef typename T::Ref reference;
    typedef mini::OffsetT difference_type;
};

template <mini::RandomAccessIteratorT T>
struct iterator_traits<T> {
    typedef std::random_access_iterator_tag iterator_category;
    typedef typename T::Value value_type;
    typedef typename T::Ptr pointer;
    typedef typename T::Ref reference;
    typedef mini::OffsetT difference_type;
};

} // namespace std