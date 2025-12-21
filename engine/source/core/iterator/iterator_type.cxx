module;

#include <iterator>

export module mini.iterator:type;

import mini.type;

namespace mini {

export template <typename T>
concept ForwardIteratorT = CopyableT<T> && EqualityComparableT<T> && //
                           !ConvertibleToT<SizeT, T> &&              //
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

export template <typename T>
concept BidrectionalIteratorT = ForwardIteratorT<T> && //
                                requires(T i)          //
{
    { i.Decrement() } -> ConvertibleToT<bool>;

    { --i } -> SameAsT<T &>;
    { i-- } -> SameAsT<T>;
};

export template <typename T>
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

export template <typename T, typename U>
concept IteratorCopyableFromT = ForwardIteratorT<T> && ForwardIteratorT<U> &&
                                requires(T i, U v) { *i = ForwardArg<typename U::Value>(*v); };

export template <typename T, typename U>
concept IteratorMovableFromT = ForwardIteratorT<T> && ForwardIteratorT<U> &&
                               requires(T i, U v) { *i = MoveArg(*v); };

export template <typename Iter, typename T>
concept ForwardIteratableByT = ForwardIteratorT<Iter> && ConvertibleToT<typename Iter::Value, T>;

export template <typename Iter, typename T>
concept BidirectionalIteratableByT = BidrectionalIteratorT<Iter> &&
                                     ConvertibleToT<typename Iter::Value, T>;

export template <typename Iter, typename T>
concept RandomAccessIteratableByT = RandomAccessIteratorT<Iter> &&
                                    ConvertibleToT<typename Iter::Value, T>;

} // namespace mini