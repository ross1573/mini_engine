module;

#include <iterator>

export module mini.core:iterator;

import :type;

namespace mini {

export template <typename T>
concept ForwardIteratorT = CopyableT<T> && EqualityComparableT<T> && //
                           !ConvertibleToT<size_t, T> &&             //
                           requires(T iter, T const other)           //
{
    requires !ReferenceT<typename T::Value>;
    requires PointerT<typename T::Pointer>;
    requires ReferenceT<typename T::Reference>;

    { iter.Valid() } -> ConvertibleToT<bool>;
    { iter.ValidWith(other) } -> ConvertibleToT<bool>;
    { iter.Increment() } -> ConvertibleToT<bool>;

    { iter.Address() } -> SameAsT<typename T::Pointer>;
    { iter.operator->() } -> SameAsT<typename T::Pointer>;

    { *iter } -> SameAsT<typename T::Reference>;
    { ++iter } -> SameAsT<T &>;
    { iter++ } -> SameAsT<T>;
};

export template <typename T>
concept BidrectionalIteratorT = ForwardIteratorT<T> && //
                                requires(T iter)       //
{
    { iter.Decrement() } -> ConvertibleToT<bool>;

    { --iter } -> SameAsT<T &>;
    { iter-- } -> SameAsT<T>;
};

export template <typename T>
concept RandomAccessIteratorT = BidrectionalIteratorT<T> && ComparableT<T> &&    //
                                requires(T iter, T const citer, offset_t offset) //
{
    { iter.Advance(offset) } -> ConvertibleToT<bool>;

    { iter += offset } -> SameAsT<T &>;
    { iter -= offset } -> SameAsT<T &>;
    { citer + offset } -> SameAsT<T>;
    { citer - offset } -> SameAsT<T>;
    { offset + citer } -> SameAsT<T>;
    { citer - iter } -> SameAsT<offset_t>;
    { citer[offset] } -> SameAsT<typename T::Reference>;
};

export template <typename T, typename U>
concept IteratorCopyableFromT = ForwardIteratorT<T> && ForwardIteratorT<U> &&
                                requires(T iter, U value) { *iter = ForwardArg<typename U::Value>(*value); };

export template <typename T, typename U>
concept IteratorMovableFromT = ForwardIteratorT<T> && ForwardIteratorT<U> && //
                               requires(T iter, U value) { *iter = MoveArg(*value); };

export template <typename Iter, typename T>
concept ForwardIteratableByT = ForwardIteratorT<Iter> && ConvertibleToT<typename Iter::Value, T>;

export template <typename Iter, typename T>
concept BidirectionalIteratableByT = BidrectionalIteratorT<Iter> && ConvertibleToT<typename Iter::Value, T>;

export template <typename Iter, typename T>
concept RandomAccessIteratableByT = RandomAccessIteratorT<Iter> && ConvertibleToT<typename Iter::Value, T>;

} // namespace mini

namespace mini {

export template <typename T>
constexpr T::Iterator begin(T &iter)
    requires ForwardIteratorT<typename T::Iterator>
{
    return iter.Begin();
}

export template <typename T>
constexpr T::Iterator end(T &iter)
    requires ForwardIteratorT<typename T::Iterator>
{
    return iter.End();
}

export template <typename T>
constexpr T::ConstIterator begin(T const &iter)
    requires ForwardIteratorT<typename T::ConstIterator>
{
    return iter.Begin();
}

export template <typename T>
constexpr T::ConstIterator end(T const &iter)
    requires ForwardIteratorT<typename T::ConstIterator>
{
    return iter.End();
}

export template <typename T>
constexpr T::ConstIterator cbegin(T const &iter)
    requires ForwardIteratorT<typename T::ConstIterator>
{
    return iter.Begin();
}

export template <typename T>
constexpr T::ConstIterator cend(T const &iter)
    requires ForwardIteratorT<typename T::ConstIterator>
{
    return iter.End();
}

} // namespace mini

export namespace std {

template <mini::ForwardIteratorT T>
struct iterator_traits<T> {
    typedef std::forward_iterator_tag iterator_category;
    typedef T::Value value_type;
    typedef T::Pointer pointer;
    typedef T::Reference reference;
    typedef mini::offset_t difference_type;
};

template <mini::BidrectionalIteratorT T>
struct iterator_traits<T> {
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef T::Value value_type;
    typedef T::Pointer pointer;
    typedef T::Reference reference;
    typedef mini::offset_t difference_type;
};

template <mini::RandomAccessIteratorT T>
struct iterator_traits<T> {
    typedef std::random_access_iterator_tag iterator_category;
    typedef T::Value value_type;
    typedef T::Pointer pointer;
    typedef T::Reference reference;
    typedef mini::offset_t difference_type;
};

} // namespace std