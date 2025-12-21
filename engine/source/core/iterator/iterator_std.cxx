module;

#include <iterator>

export module mini.iterator:std;

import mini.type;
import :type;

namespace mini {

export template <typename T>
inline constexpr typename T::Iterator begin(T &c)
    requires ForwardIteratorT<typename T::Iterator>
{
    return c.Begin();
}

export template <typename T>
inline constexpr typename T::Iterator end(T &c)
    requires ForwardIteratorT<typename T::Iterator>
{
    return c.End();
}

export template <typename T>
inline constexpr typename T::ConstIterator cbegin(T const &c)
    requires ForwardIteratorT<typename T::ConstIterator>
{
    return c.Begin();
}

export template <typename T>
inline constexpr typename T::ConstIterator cend(T const &c)
    requires ForwardIteratorT<typename T::ConstIterator>
{
    return c.End();
}

} // namespace mini

namespace std {

export template <mini::ForwardIteratorT T>
struct iterator_traits<T> {
    typedef std::forward_iterator_tag iterator_category;
    typedef typename T::Value value_type;
    typedef typename T::Ptr pointer;
    typedef typename T::Ref reference;
    typedef mini::OffsetT difference_type;
};

export template <mini::BidrectionalIteratorT T>
struct iterator_traits<T> {
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef typename T::Value value_type;
    typedef typename T::Ptr pointer;
    typedef typename T::Ref reference;
    typedef mini::OffsetT difference_type;
};

export template <mini::RandomAccessIteratorT T>
struct iterator_traits<T> {
    typedef std::random_access_iterator_tag iterator_category;
    typedef typename T::Value value_type;
    typedef typename T::Ptr pointer;
    typedef typename T::Ref reference;
    typedef mini::OffsetT difference_type;
};

} // namespace std