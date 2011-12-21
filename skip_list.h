//==============================================================================
// skip_list.h
// Copyright (c) 2011 Pete Goodliffe. All rights reserved.
//==============================================================================

#pragma once

#include "skip_list_detail.h"

#include <memory>     // for std::allocator
#include <functional> // for std::less
#include <iterator>   // for std::reverse_iterator
#include <utility>    // for std::pair

//==============================================================================

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning (disable : 4068 ) /* disable unknown pragma warnings */
#endif

//==============================================================================
#pragma mark - skip_list
//==============================================================================

namespace goodliffe {

/// An STL-style skip list container; a reasonably fast associative
/// ordered container.
///
/// The skip list provides fast searching, and good insert/erase performance.
/// You can iterate bi-directionally, but do not have full random access.
///
/// For full random access, use the random_access_skip_list container.
///
/// TODO:
///     * C++11: noexcept decls
///     * C++11: move operations
///     * make "multi" skip_list version
///
/// Document:
///     * efficiency of operations (big-O notation)
///     * iterator invalidation
///
/// Following the freaky STL container names, this might be better named
/// unique_sorted_list or sorted_list, or somesuch other drivel.
///
/// @param T         Template type for kind of object held in the container.
/// @param Compare   Template type describing the ordering comparator.
///                  Defaults to useing the less than operator.
/// @param Allocator Template type for memory allocator for the contents of
///                  of the container. Defaults to a standard std::allocator
template <typename T,
          typename Compare        = std::less<T>,
          typename Allocator      = std::allocator<T>,
          typename LevelGenerator = detail::skip_list_level_generator<32>,
          typename SkipListType   = typename detail::skip_list_impl<T,Compare,Allocator,LevelGenerator,detail::skip_list_node<T> >,
          template <typename> class Iterator = detail::skip_list_iterator >
class skip_list
{
protected:
    typedef skip_list
        <
            T,Compare,Allocator,LevelGenerator,SkipListType,Iterator
        >
        self_type;

    typedef SkipListType                    impl_type;
    typedef typename impl_type::node_type   node_type;

    template <typename T1> friend class detail::skip_list_iterator;
    template <typename T1> friend class detail::skip_list_const_iterator;
    template <typename T1> friend class detail::random_access_skip_list_iterator;
    template <typename T1> friend class detail::random_access_skip_list_const_iterator;

public:

    //======================================================================
    // types

    typedef T                                           value_type;
    typedef Allocator                                   allocator_type;
    typedef typename impl_type::size_type               size_type;
    typedef typename allocator_type::difference_type    difference_type;
    typedef typename allocator_type::reference          reference;
    typedef typename allocator_type::const_reference    const_reference;
    typedef typename allocator_type::pointer            pointer;
    typedef typename allocator_type::const_pointer      const_pointer;
    typedef Compare                                     compare;

    typedef Iterator<impl_type>                         iterator;
    typedef typename iterator::const_iterator           const_iterator;
    typedef std::reverse_iterator<iterator>             reverse_iterator;
    typedef std::reverse_iterator<const_iterator>       const_reverse_iterator;

    //======================================================================
    // lifetime management

    explicit skip_list(const Allocator &alloc = Allocator());

    template <class InputIterator>
    skip_list(InputIterator first, InputIterator last, const Allocator &alloc = Allocator());

    skip_list(const skip_list &other);
    skip_list(const skip_list &other, const Allocator &alloc);

    // C++11
    //skip_list(const skip_list &&other);
    //skip_list(const skip_list &&other, const Allocator &alloc);
    //skip_list(std::initializer_list<T> init, const Allocator &alloc = Allocator());

    ~skip_list();

    allocator_type get_allocator() const;

    //======================================================================
    // assignment

    skip_list &operator=(const skip_list &other);
    //C++11 skip_list& operator=(skip_list&& other);

    template <typename InputIterator>
    void assign(InputIterator first, InputIterator last);

    //======================================================================
    // element access

    reference       front();
    const_reference front() const;
    reference       back();
    const_reference back() const;

    //======================================================================
    // iterators

    iterator       begin();
    const_iterator begin() const;
    const_iterator cbegin() const;

    iterator       end();
    const_iterator end() const;
    const_iterator cend() const;

    reverse_iterator       rbegin();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const;

    reverse_iterator       rend();
    const_reverse_iterator rend() const;
    const_reverse_iterator crend() const;

    //======================================================================
    // capacity

    bool      empty() const;
    size_type size() const;
    size_type max_size() const;

    //======================================================================
    // modifiers

    void clear();
    
    typedef typename std::pair<iterator,bool> insert_by_value_result;

    insert_by_value_result insert(const value_type &value);
    iterator insert(const_iterator hint, const value_type &value);

    //C++11iterator insert(value_type &&value);
    //C++11iterator insert(const_iterator hint, const value_type &&value);

    template <class InputIterator>
    void insert(InputIterator first, InputIterator last);

    //C++11iterator insert(std::initializer_list<value_type> ilist);
    //C++11emplace

    size_type erase(const value_type &value);
    iterator  erase(const_iterator position);
    iterator  erase(const_iterator first, const_iterator last);

    void swap(skip_list &other);

    friend void swap(skip_list &lhs, skip_list &rhs) { lhs.swap(rhs); }

    //======================================================================
    // lookup

    bool           contains(const value_type &value) const { return count(value) != 0; }
    size_type      count(const value_type &value) const;

    iterator       find(const value_type &value);
    const_iterator find(const value_type &value) const;

    //======================================================================
    // other operations

    // std::list has:
    //   * merge
    //   * splice
    //   * remove
    //   * remove_if
    //   * reverse
    //   * unique
    //   * sort
    
    template <typename STREAM>
    void dump(STREAM &stream) const { impl.dump(stream); }

protected:
    impl_type impl;
};

template <class T, class C, class A, class LG, class SLT, template <class> class I>
bool operator==(const skip_list<T,C,A,LG,SLT,I> &lhs, const skip_list<T,C,A,LG,SLT,I> &rhs);

template <class T, class C, class A, class LG, class SLT, template <class> class I>
bool operator!=(const skip_list<T,C,A,LG,SLT,I> &lhs, const skip_list<T,C,A,LG,SLT,I> &rhs);

template <class T, class C, class A, class LG, class SLT, template <class> class I>
bool operator<(const skip_list<T,C,A,LG,SLT,I> &lhs, const skip_list<T,C,A,LG,SLT,I> &rhs);

template <class T, class C, class A, class LG, class SLT, template <class> class I>
bool operator<=(const skip_list<T,C,A,LG,SLT,I> &lhs, const skip_list<T,C,A,LG,SLT,I> &rhs);

template <class T, class C, class A, class LG, class SLT, template <class> class I>
bool operator>(const skip_list<T,C,A,LG,SLT,I> &lhs, const skip_list<T,C,A,LG,SLT,I> &rhs);

template <class T, class C, class A, class LG, class SLT, template <class> class I>
bool operator>=(const skip_list<T,C,A,LG,SLT,I> &lhs, const skip_list<T,C,A,LG,SLT,I> &rhs);
    
} // namespace goodliffe

namespace std
{
    template <class T, class C, class A, class LG, class SLT, template <class> class I>
    void swap(goodliffe::skip_list<T,C,A,LG,SLT,I> &lhs, goodliffe::skip_list<T,C,A,LG,SLT,I> &rhs);
}

//==============================================================================
#pragma mark - skip_list_impl declaration
//==============================================================================

namespace goodliffe {
namespace detail {

template <typename T>
struct skip_list_node
{
    typedef skip_list_node<T> self_type;

#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    unsigned    magic;
#endif
    T           value;
    unsigned    level;
    self_type  *prev;
    self_type **next; ///< effectively node_type *next[level+1];
};

template <typename T, typename Allocator>
struct skip_list_node_traits;

/// Internal implementation of skip_list data structure and methods for
/// modifying it.
///
/// Not for "public" access.
///
/// @internal
template <typename T, typename Compare, typename Allocator,
          typename LevelGenerator, typename NodeType>
class skip_list_impl
{
public:
    
    typedef T                                   value_type;
    typedef typename Allocator::size_type       size_type;
    typedef typename Allocator::difference_type difference_type;
    typedef Allocator                           allocator_type;
    typedef Compare                             compare_type;
    typedef LevelGenerator                      generator_type;
    typedef NodeType                            node_type;

    static const unsigned num_levels = LevelGenerator::num_levels;
    typedef typename Allocator::const_reference const_reference;
    typedef typename Allocator::const_pointer   const_pointer;

    skip_list_impl(const Allocator &alloc = Allocator());
    ~skip_list_impl();

    Allocator        get_allocator() const;
    size_type        size() const                          { return item_count; }
    bool             is_valid(const node_type *node) const { return node && node != head && node != tail; }
    node_type       *front()                               { return head->next[0]; }
    const node_type *front() const                         { return head->next[0]; }
    node_type       *one_past_end()                        { return tail; }
    const node_type *one_past_end() const                  { return tail; }
    node_type       *find(const value_type &value) const;
    node_type       *at(size_type index);
    const node_type *at(size_type index) const;
    node_type       *insert(const value_type &value, node_type *hint = 0);
    void             remove(node_type *value);
    void             remove_all();
    void             remove_between(node_type *first, node_type *last);
    void             swap(skip_list_impl &other);
    size_type        index_of(const node_type *node) const;

    template <typename STREAM>
    void        dump(STREAM &stream) const;
    bool        check() const;
    unsigned    new_level();

    compare_type less;

private:
    typedef skip_list_node_traits<node_type, Allocator> node_traits;

    skip_list_impl(const skip_list_impl &other);
    skip_list_impl &operator=(const skip_list_impl &other);
    
    size_type find_chain(const value_type &value, node_type **chain) const;
    size_type find_chain(const value_type &value, node_type **chain, size_type *indexes) const;
    size_type find_chain(const node_type *node, node_type **chain, size_type *indexes) const;
    size_type find_end_chain(node_type **chain, size_type *indexes) const;

    allocator_type  alloc;
    generator_type  generator;
    unsigned        levels;
    node_type      *head;
    node_type      *tail;
    size_type       item_count;
};

} // namespace detail
} // namespace goodliffe

//==============================================================================
#pragma mark - iterators
//==============================================================================

namespace goodliffe {
namespace detail {

template <typename SKIP_LIST_IMPL>
class skip_list_iterator
    : public std::iterator<std::bidirectional_iterator_tag,
                           typename SKIP_LIST_IMPL::value_type,
                           typename SKIP_LIST_IMPL::difference_type,
                           typename SKIP_LIST_IMPL::const_pointer,
                           typename SKIP_LIST_IMPL::const_reference>
{
public:
    typedef SKIP_LIST_IMPL                              impl_type;
    typedef skip_list_const_iterator<SKIP_LIST_IMPL>    const_iterator;
    typedef typename impl_type::node_type               node_type;
    typedef skip_list_iterator<SKIP_LIST_IMPL>          self_type;

    skip_list_iterator() :
#ifdef SKIP_LIST_DIAGNOSTICS
        impl(0),
#endif
        node(0) {}
    skip_list_iterator(impl_type *impl_, node_type *node_) :
#ifdef SKIP_LIST_DIAGNOSTICS
        impl(impl_),
#endif
        node(node_) {}

    self_type &operator++()
        { node = node->next[0]; return *this; }
    self_type operator++(int) // postincrement
        { self_type old(*this); node = node->next[0]; return old; }

    self_type &operator--()
        { node = node->prev; return *this; }
    self_type operator--(int) // postdecrement
        { self_type old(*this); node = node->prev; return old; }

    typename impl_type::const_reference operator*()  { return node->value; }
    typename impl_type::const_pointer   operator->() { return node->value; }
    
    bool operator==(const self_type &other) const
        { return node == other.node; }
    bool operator!=(const self_type &other) const
        { return !operator==(other); }
    
    bool operator==(const const_iterator &other) const
        { return node == other.get_node(); }
    bool operator!=(const const_iterator &other) const
        { return !operator==(other); }

#ifdef SKIP_LIST_DIAGNOSTICS
    const impl_type *get_impl() const { return impl; } ///< @internal
#endif
    const node_type *get_node() const { return node; } ///< @internal

private:
#ifdef SKIP_LIST_DIAGNOSTICS
    impl_type *impl;
#endif
    node_type *node;
};

template <class SKIP_LIST_IMPL>
class skip_list_const_iterator
    : public std::iterator<std::bidirectional_iterator_tag,
                           typename SKIP_LIST_IMPL::value_type,
                           typename SKIP_LIST_IMPL::difference_type,
                           typename SKIP_LIST_IMPL::const_pointer,
                           typename SKIP_LIST_IMPL::const_reference>
{
public:
    typedef const SKIP_LIST_IMPL                        impl_type;
    typedef skip_list_iterator<SKIP_LIST_IMPL>          iterator;
    typedef const typename impl_type::node_type         node_type;
    typedef skip_list_const_iterator<SKIP_LIST_IMPL>    self_type;

    skip_list_const_iterator() :
#ifdef SKIP_LIST_DIAGNOSTICS
        impl(0),
#endif
        node(0) {}
    skip_list_const_iterator(const iterator &i) :
#ifdef SKIP_LIST_DIAGNOSTICS
        impl(i.get_impl()),
#endif
        node(i.get_node()) {}
    skip_list_const_iterator(const impl_type *impl_, node_type *node_) :
#ifdef SKIP_LIST_DIAGNOSTICS
        impl(impl_),
#endif
        node(node_) {}

    self_type &operator++()
        { node = node->next[0]; return *this; }
    self_type operator++(int) // postincrement
        { self_type old(*this); node = node->next[0]; return old; }

    self_type &operator--()
        { node = node->prev; return *this; }
    self_type operator--(int) // postdecrement
        { self_type old(*this); node = node->prev; return old; }

    typename impl_type::const_reference operator*()  { return node->value; }
    typename impl_type::const_pointer   operator->() { return node->value; }

    bool operator==(const self_type &other) const
        { return node == other.node; }
    bool operator!=(const self_type &other) const
        { return !operator==(other); }

    bool operator==(const iterator &other) const
        { return node == other.get_node(); }
    bool operator!=(const iterator &other) const
        { return !operator==(other); }

#ifdef SKIP_LIST_DIAGNOSTICS
    const impl_type *get_impl() const { return impl; } ///< @internal
#endif
    const node_type *get_node() const { return node; } ///< @internal

private:
#ifdef SKIP_LIST_DIAGNOSTICS
    impl_type *impl;
#endif
    node_type *node;
};

} // namespace detail
} // namespace goodliffe

//==============================================================================
#pragma mark - lifetime management
//==============================================================================

namespace goodliffe {

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
skip_list<T,C,A,LG,SLT,I>::skip_list(const allocator_type &alloc_)
:   impl(alloc_)
{
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
skip_list<T,C,A,LG,SLT,I>::~skip_list()
{
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
template <class InputIterator>
inline
skip_list<T,C,A,LG,SLT,I>::skip_list(InputIterator first, InputIterator last, const allocator_type &alloc_)
:   impl(alloc_)
{
    assign(first, last);
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
skip_list<T,C,A,LG,SLT,I>::skip_list(const skip_list &other)
:   impl(other.get_allocator())
{    
    assign(other.begin(), other.end());
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
skip_list<T,C,A,LG,SLT,I>::skip_list(const skip_list &other, const allocator_type &alloc_)
:   impl(alloc_)
{
    assign(other.begin(), other.end());
}

// C++11
//skip_list(const skip_list &&other);
//skip_list(const skip_list &&other, const Allocator &alloc);
//skip_list(std::initializer_list<T> init, const Allocator &alloc = Allocator());

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
A skip_list<T,C,A,LG,SLT,I>::get_allocator() const
{
    return impl.get_allocator();
}

//==============================================================================
#pragma mark assignment

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
skip_list<T,C,A,LG,SLT,I> &
skip_list<T,C,A,LG,SLT,I>::operator=(const skip_list<T,C,A,LG,SLT,I> &other)
{
    assign(other.begin(), other.end());
    return *this;
}

//C++11 skip_list& operator=(skip_list&& other);

template <class T, class C, class A, class LG, class SLT, template <class> class I>
template <typename InputIterator>
inline
void skip_list<T,C,A,LG,SLT,I>::assign(InputIterator first, InputIterator last)
{
    clear();
    while (first != last) insert(*first++);
}

//==============================================================================
#pragma mark element access

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::reference
skip_list<T,C,A,LG,SLT,I>::front()
{
    assert_that(!empty());
    return impl.front()->value;
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::const_reference
skip_list<T,C,A,LG,SLT,I>::front() const
{
    assert_that(!empty());
    return impl.front()->value;
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::reference
skip_list<T,C,A,LG,SLT,I>::back()
{
    assert_that(!empty());
    return impl.one_past_end()->prev->value;
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::const_reference
skip_list<T,C,A,LG,SLT,I>::back() const
{
    assert_that(!empty());
    return impl.one_past_end()->prev->value;
}

//==============================================================================
#pragma mark iterators

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::iterator
skip_list<T,C,A,LG,SLT,I>::begin()
{
    return iterator(&impl, impl.front());
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::const_iterator
skip_list<T,C,A,LG,SLT,I>::begin() const
{
    return const_iterator(&impl, impl.front());
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::const_iterator
skip_list<T,C,A,LG,SLT,I>::cbegin() const
{
    return const_iterator(&impl, impl.front());
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::iterator
skip_list<T,C,A,LG,SLT,I>::end()
{
    return iterator(&impl, impl.one_past_end());
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::const_iterator
skip_list<T,C,A,LG,SLT,I>::end() const
{
    return const_iterator(&impl, impl.one_past_end());
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::const_iterator
skip_list<T,C,A,LG,SLT,I>::cend() const
{
    return const_iterator(&impl, impl.one_past_end());
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::reverse_iterator
skip_list<T,C,A,LG,SLT,I>::rbegin()
{
    return reverse_iterator(end());
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::const_reverse_iterator
skip_list<T,C,A,LG,SLT,I>::rbegin() const
{
    return const_reverse_iterator(end());
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::const_reverse_iterator
skip_list<T,C,A,LG,SLT,I>::crbegin() const
{
    return const_reverse_iterator(end());
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::reverse_iterator
skip_list<T,C,A,LG,SLT,I>::rend()
{
    return reverse_iterator(begin());
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::const_reverse_iterator
skip_list<T,C,A,LG,SLT,I>::rend() const
{
    return const_reverse_iterator(begin());
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::const_reverse_iterator
skip_list<T,C,A,LG,SLT,I>::crend() const
{
    return const_reverse_iterator(begin());
}

//==============================================================================
#pragma mark capacity

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
bool skip_list<T,C,A,LG,SLT,I>::empty() const
{
    return impl.size() == 0;
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::size_type
skip_list<T,C,A,LG,SLT,I>::size() const
{
    return impl.size();
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::size_type
skip_list<T,C,A,LG,SLT,I>::max_size() const
{
    return impl.get_allocator().max_size();
}

//==============================================================================
#pragma mark modifiers

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
void skip_list<T,C,A,LG,SLT,I>::clear()
{
    impl.remove_all();
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::insert_by_value_result
skip_list<T,C,A,LG,SLT,I>::insert(const value_type &value)
{
    node_type *node = impl.insert(value);
    return std::make_pair(iterator(&impl, node), impl.is_valid(node));
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::iterator
skip_list<T,C,A,LG,SLT,I>::insert(const_iterator hint, const value_type &value)
{
    assert_that(hint.get_impl() == &impl);
    
    const node_type *hint_node = hint.get_node();

    if (impl.is_valid(hint_node) && detail::less_or_equal(value, hint_node->value, impl.less))
        return iterator(&impl,impl.insert(value)); // bad hint, resort to "normal" insert
    else
        return iterator(&impl,impl.insert(value,const_cast<node_type*>(hint_node)));
}

//C++11iterator insert const_iterator pos, value_type &&value);

template <class T, class C, class A, class LG, class SLT, template <class> class I>
template <class InputIterator>
inline
void
skip_list<T,C,A,LG,SLT,I>::insert(InputIterator first, InputIterator last)
{
    iterator last_inserted = end();
    while (first != last)
    {
        last_inserted = insert(last_inserted, *first++);
    }
}

//C++11iterator insert(std::initializer_list<value_type> ilist);
// C++11 emplace

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::size_type
skip_list<T,C,A,LG,SLT,I>::erase(const value_type &value)
{
    node_type *node = impl.find(value);
    if (impl.is_valid(node) && detail::equivalent(node->value, value, impl.less))
    {
        impl.remove(node);
        return 1;
    }
    else
    {
        return 0;
    }
}    

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::iterator
skip_list<T,C,A,LG,SLT,I>::erase(const_iterator position)
{
    assert_that(position.get_impl() == &impl);
    assert_that(impl.is_valid(position.get_node()));
    node_type *node = const_cast<node_type*>(position.get_node());
    node_type *next = node->next[0];
    impl.remove(node);
    return iterator(&impl, next);
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::iterator
skip_list<T,C,A,LG,SLT,I>::erase(const_iterator first, const_iterator last)
{
    assert_that(first.get_impl() == &impl);
    assert_that(last.get_impl() == &impl);

    if (first != last)
    {
        node_type *first_node = const_cast<node_type*>(first.get_node());
        node_type *last_node  = const_cast<node_type*>(last.get_node()->prev);
        impl.remove_between(first_node, last_node);
    }
    
    return iterator(&impl, const_cast<node_type*>(last.get_node()));
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
void
skip_list<T,C,A,LG,SLT,I>::swap(skip_list<T,C,A,LG,SLT,I> &other)
{
    impl.swap(other.impl);
}
    
//==============================================================================
#pragma mark lookup

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::size_type
skip_list<T,C,A,LG,SLT,I>::count(const value_type &value) const
{
    const node_type *node = impl.find(value);
    return impl.is_valid(node) && detail::equivalent(node->value, value, impl.less);
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::iterator
skip_list<T,C,A,LG,SLT,I>::find(const value_type &value)
{
    node_type *node = impl.find(value);
    return impl.is_valid(node) && detail::equivalent(node->value, value, impl.less)
        ? iterator(&impl, node)
        : end();
}
  
template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,LG,SLT,I>::const_iterator
skip_list<T,C,A,LG,SLT,I>::find(const value_type &value) const
{
    const node_type *node = impl.find(value);
    return impl.is_valid(node) && detail::equivalent(node->value, value, impl.less)
        ? const_iterator(&impl, node)
        : end();
}
    
} // namespace goodliffe

//==============================================================================
#pragma mark - non-members
//==============================================================================

namespace goodliffe {

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
bool operator==(const skip_list<T,C,A,LG,SLT,I> &lhs, const skip_list<T,C,A,LG,SLT,I> &rhs)
{
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
bool operator!=(const skip_list<T,C,A,LG,SLT,I> &lhs, const skip_list<T,C,A,LG,SLT,I> &rhs)
{
    return !operator==(lhs, rhs);
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
bool operator<(const skip_list<T,C,A,LG,SLT,I> &lhs, const skip_list<T,C,A,LG,SLT,I> &rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
bool operator<=(const skip_list<T,C,A,LG,SLT,I> &lhs, const skip_list<T,C,A,LG,SLT,I> &rhs)
{
    return !(rhs < lhs);
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
bool operator>(const skip_list<T,C,A,LG,SLT,I> &lhs, const skip_list<T,C,A,LG,SLT,I> &rhs)
{
    return rhs < lhs;
}

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
bool operator>=(const skip_list<T,C,A,LG,SLT,I> &lhs, const skip_list<T,C,A,LG,SLT,I> &rhs)
{
    return !(lhs < rhs);
}
    
} // namespace goodliffe

template <class T, class C, class A, class LG, class SLT, template <class> class I>
inline
void std::swap(goodliffe::skip_list<T,C,A,LG,SLT,I> &lhs, goodliffe::skip_list<T,C,A,LG,SLT,I> &rhs)
{
    lhs.swap(rhs);
}

//==============================================================================
#pragma mark - skip_list_node_traits
//==============================================================================

namespace goodliffe {
namespace detail {

#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
enum
{
    MAGIC_GOOD = 0x01020304,
    MAGIC_BAD  = 0xfefefefe
};
#endif

// TODO: change these to use correct types (not unsigned, int)

/// The functions to allocate and deallocate a skip_list_node
template <typename NodeType, typename Allocator>
struct skip_list_node_traits
{
    typedef typename Allocator::template rebind<NodeType>::other  NodeAllocator;
    typedef typename Allocator::template rebind<NodeType*>::other ListAllocator;
    typedef typename Allocator::difference_type                   Difference;
    typedef typename Allocator::size_type                         Size;

    static
    inline
    NodeType *allocate(unsigned level, Allocator &alloc)
    {
        NodeType *node = NodeAllocator(alloc).allocate(1, (void*)0);
        node->next  = ListAllocator(alloc).allocate(level+1, (void*)0);
        node->level = level;
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
        for (unsigned n = 0; n <= level; ++n) node->next[n] = 0;
        node->magic = MAGIC_GOOD;
#endif
        return node;
    }
    static
    inline
    void deallocate(NodeType *node, Allocator &alloc)
    {
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
        assert_that(node->magic == MAGIC_GOOD);
        node->magic = MAGIC_BAD;
        for (unsigned n = 0; n <= node->level; ++n) node->next[n] = 0;
        node->prev = 0;
#endif
        ListAllocator(alloc).deallocate(node->next, node->level+1);
        NodeAllocator(alloc).deallocate(node, 1);
    }

    static void increment_span(NodeType *node, unsigned level) {}
    static void change_span(NodeType *node, unsigned level, Difference by) {}
    static void set_span(NodeType *node, unsigned level, Size span) {}
    static void set_all_spans(NodeType *node, Size span) {}
    static void decrement_span(NodeType *node, Size level) {}
    static Size span(const NodeType *node, unsigned level) { return 0; }
};
    
} // namespace detail
} // namespace goodliffe

//==============================================================================
#pragma mark - skip_list_impl
//==============================================================================

namespace goodliffe {
namespace detail {

template <class T, class C, class A, class LG, class N>
inline
skip_list_impl<T,C,A,LG,N>::skip_list_impl(const allocator_type &alloc_)
:   alloc(alloc_),
    levels(0),
    head(node_traits::allocate(num_levels, alloc)),
    tail(node_traits::allocate(num_levels, alloc)),
    item_count(0)
{
    for (unsigned n = 0; n < num_levels; n++)
    {
        head->next[n] = tail;
        tail->next[n] = 0;
        node_traits::set_span(head, n, 1);
    }
    head->prev = 0;
    tail->prev = head;
}

template <class T, class C, class A, class LG, class N>
inline
skip_list_impl<T,C,A,LG,N>::~skip_list_impl()
{
    remove_all();
    node_traits::deallocate(head, alloc);
    node_traits::deallocate(tail, alloc);
}

template <class T, class C, class A, class LG, class N>
inline
A skip_list_impl<T,C,A,LG,N>::get_allocator() const
{
    return alloc;
}

template <class T, class C, class A, class LG, class N>
inline
typename skip_list_impl<T,C,A,LG,N>::node_type *
skip_list_impl<T,C,A,LG,N>::find(const value_type &value) const
{
    // I could have a const and non-const overload, but this cast is simpler
    node_type *search = const_cast<node_type*>(head);
    for (unsigned l = levels; l; )
    {
        --l;
        while (search->next[l] != tail && detail::less_or_equal(search->next[l]->value, value, less))
        {
            search = search->next[l];
        }
    }
    return search;
}

template <class T, class C, class A, class LG, class N>
inline
typename skip_list_impl<T,C,A,LG,N>::size_type
skip_list_impl<T,C,A,LG,N>::find_chain(const value_type &value, node_type **chain, size_type *indexes) const
{
    size_type index = 0;
    node_type *cur = head;
    unsigned l = num_levels;
    while (l)
    {
        --l;
        impl_assert_that(l <= cur->level);
        while (cur->next[l] != tail && less(cur->next[l]->value, value))
        {
            index += node_traits::span(cur, l);
            cur = cur->next[l];
            impl_assert_that(l <= cur->level);
        }
        chain[l]   = cur;
        indexes[l] = index;
    }
    
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    for (unsigned l1 = 0; l1 < num_levels; ++l1)
    {
        assert_that(chain[l1]->level >= l1);
    }
#endif

    return index;
}

// TODO: fold these down
template <class T, class C, class A, class LG, class N>
inline
typename skip_list_impl<T,C,A,LG,N>::size_type
skip_list_impl<T,C,A,LG,N>::find_chain(const node_type *node, node_type **chain, size_type *indexes) const
{
    assert_that(is_valid(node));
    size_type index = 0;
    node_type *cur = head;
    unsigned l = num_levels;
    while (l)
    {
        --l;
        impl_assert_that(l <= cur->level);
        while (cur->next[l] != tail && less(cur->next[l]->value, node->value))
        {
            index += node_traits::span(cur, l);
            cur = cur->next[l];
            impl_assert_that(l <= cur->level);
        }
        chain[l]   = cur;
        indexes[l] = index;
    }
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    for (unsigned l1 = 0; l1 < num_levels; ++l1)
    {
        assert_that(chain[l1]->level >= l1);
    }
#endif
    return index;
}

// TODO: fold these down, up, sideways
template <class T, class C, class A, class LG, class N>
inline
typename skip_list_impl<T,C,A,LG,N>::size_type
skip_list_impl<T,C,A,LG,N>::find_end_chain(node_type **chain, size_type *indexes) const
{
    size_type index = 0;
    node_type *cur = head;
    unsigned l = num_levels;
    while (l)
    {
        --l;
        impl_assert_that(l <= cur->level);
        while (cur->next[l] != tail)
        {
            index += node_traits::span(cur, l);
            cur = cur->next[l];
            impl_assert_that(l <= cur->level);
        }
        chain[l]   = cur;
        indexes[l] = index;
    }
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    for (unsigned l1 = 0; l1 < num_levels; ++l1)
    {
        impl_assert_that(chain[l1]->level >= l1);
    }
#endif
    return index;
}

// TODO: Hint is now ignored (has to be, for spans to work)
template <class T, class C, class A, class LG, class N>
inline
typename skip_list_impl<T,C,A,LG,N>::node_type*
skip_list_impl<T,C,A,LG,N>::insert(const value_type &value, node_type *hint)
{
    const unsigned level = new_level();

    node_type *new_node = node_traits::allocate(level, alloc);
    assert_that(new_node);
    impl_assert_that(new_node->level == level);
    alloc.construct(&new_node->value, value);

    node_type *chain[num_levels]   = {0};
    size_type  indexes[num_levels] = {0};
    size_type  index               = find_chain(value, chain, indexes);

    // Do not allow repeated values in the list (we could in a "multi_skip_list")
    {
        node_type *next = chain[0]->next[0];
        if (next != tail && detail::equivalent(next->value, value, less))
            return tail;
    }

    for (unsigned l = 0; l < num_levels; ++l)
    {
        if (l > level)
        {
            node_traits::increment_span(chain[l], l);
        }
        else
        {
            new_node->next[l] = chain[l]->next[l];
            chain[l]->next[l] = new_node;
            size_type prev_span = node_traits::span(chain[l], l);
            node_traits::set_span(chain[l], l, index+1-indexes[l]);
            node_traits::set_span(new_node, l, prev_span - (index-indexes[l]));
        }
    }
    new_node->next[0]->prev = new_node;
    new_node->prev          = chain[0];
    
    ++item_count;

#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    check();
#endif

    return new_node;
}

template <class T, class C, class A, class LG, class N>
inline
void
skip_list_impl<T,C,A,LG,N>::remove(node_type *node)
{
    assert_that(is_valid(node));
    assert_that(node->next[0]);
    
    node_type *chain[num_levels]   = {0};
    size_type  indexes[num_levels] = {0};
    find_chain(node, chain, indexes);

    node->next[0]->prev = node->prev;
    
    for (unsigned l = 0; l < num_levels; ++l)
    {
        if (chain[l]->next[l] == node)
        {
            node_traits::set_span(chain[l], l, node_traits::span(chain[l], l) + node_traits::span(node, l)-1);
            chain[l]->next[l] = node->next[l];
        }
        else if (chain[l] == head || less(chain[l]->value, node->value))
        {
            node_traits::decrement_span(chain[l], l);
        }
    }

    alloc.destroy(&node->value);
    node_traits::deallocate(node, alloc);

    item_count--;
    
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    check();
#endif
}

template <class T, class C, class A, class LG, class N>
inline
void
skip_list_impl<T,C,A,LG,N>::remove_all()
{
    node_type *node = head->next[0];
    while (node != tail)
    {
        node_type *next = node->next[0];
        alloc.destroy(&node->value);
        node_traits::deallocate(node, alloc);
        node = next;
    }

    for (unsigned l = 0; l < num_levels; ++l)
    {
        head->next[l] = tail;
        node_traits::set_span(head, l, 1);
    }
    tail->prev = head;
    item_count = 0;
        
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    check();
#endif
}

template <class T, class C, class A, class LG, class N>
inline
void 
skip_list_impl<T,C,A,LG,N>::remove_between(node_type *first, node_type *last)
{
    assert_that(is_valid(first));
    assert_that(is_valid(last));

    node_type * const prev         = first->prev;
    node_type * const one_past_end = last->next[0];

    node_type *first_chain[num_levels]   = {0};
    node_type *last_chain[num_levels]    = {0};
    size_type  first_indexes[num_levels] = {0};
    size_type  last_indexes[num_levels]  = {0};
    size_type  first_index               = find_chain(first, first_chain, first_indexes);
    size_type  last_index                = one_past_end != tail
                                         ? find_chain(one_past_end, last_chain, last_indexes)
                                         : find_end_chain(last_chain, last_indexes);
    size_type  size_reduction            = last_index-first_index;
/*
    std::cerr << "Removing from " << first->value << " to " << last->value;
    for (int n = 0; n < levels; ++n)
        std::cerr << "[" << n << "] first=" << first_chain[n]->value
        //<< "/" << first_indexes[n]
            << ", last=" << last_chain[n]->value
        //<< "/" << last_indexes[n]
            << "  span=" << node_traits::span(last_chain[n], n) << "\n";
*/
    unsigned last_node_level = 0;
    while (last_node_level+1 < num_levels
           && last_chain[last_node_level+1] == last) ++last_node_level;
    impl_assert_that(last_node_level == last->level);

    // backwards pointer
    one_past_end->prev = prev;

    // forwards pointers (and spans)
    for (unsigned l = 0; l < num_levels; ++l)
    {
        // forwards pointer
        if (l <= last_node_level)
            first_chain[l]->next[l] = last->next[l];
        else
            first_chain[l]->next[l] = last_chain[l]->next[l];

        // span
        node_traits::set_span(first_chain[l], l, last_indexes[l]+node_traits::span(last_chain[l], l)-first_indexes[l]-size_reduction);
    }

    // now delete all the nodes between [first,last]
    while (first != one_past_end)
    {
        node_type *next = first->next[0];
        alloc.destroy(&first->value);
        node_traits::deallocate(first, alloc);
        item_count--;
        first = next;
    }
        
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    check();
#endif
}

template <class T, class C, class A, class LG, class N>
inline
unsigned skip_list_impl<T,C,A,LG,N>::new_level()
{    
    unsigned level = generator.new_level();
    if (level >= levels)
    {
        level = levels;
        ++levels;
    }
    return level;
}

template <class T, class C, class A, class LG, class N>
inline
void skip_list_impl<T,C,A,LG,N>::swap(skip_list_impl &other)
{
    using std::swap;

    swap(alloc,      other.alloc);
    swap(less,       other.less);
    swap(generator,  other.generator);
    swap(levels,     other.levels);
    swap(head,       other.head);
    swap(tail,       other.tail);
    swap(item_count, other.item_count);

#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    check();
#endif
}

// for diagnostics only
template <class T, class C, class A, class LG, class N>
template <class STREAM>
inline
void skip_list_impl<T,C,A,LG,N>::dump(STREAM &s) const
{
    s << "skip_list(size="<<item_count<<",levels=" << levels << ")\n";
    for (unsigned l = 0; l < levels+1; ++l)
    {
        s << "  [" << l << "]" ;
        const node_type *n = head;
        while (n)
        {
            impl_assert_that(l <= n->level);
            const node_type *next = n->next[l];
            size_type span = node_traits::span(n, l);
            bool prev_ok = false;
            char prev_char = span > 1 ? '(' : 'X';
            if (next && span <= 1)
            {
                if (next->prev == n)
                {
                    prev_ok = true;
                    prev_char = '<';
                }
            }
            if (is_valid(n))
                s << n->value << " ";
            else
                s << "* ";
            
            if (n != tail)
            {
                if (next != tail && !less(n->value, next->value))
                    s << "*XXXXXXXXX* ";
                s << span << ">"
                  << " "
                  << prev_char;
            }
            n = next;
        }
        s << "\n";
    }
}

#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
// for diagnostics only
template <class T, class C, class A, class LG, class N>
inline
bool skip_list_impl<T,C,A,LG,N>::check() const
{
    for (unsigned l = 0; l < levels; ++l)
    {
        unsigned count = 0;
        const node_type *n = head;

        while (n != tail)
        {
            if (n->magic != MAGIC_GOOD)
            {
                assert_that(false && "bad magic");
                dump(std::cerr);
                return false;
            }

            // if level 0, we check prev pointers
            if (l == 0 && n->next[0]->prev != n)
            {
                assert_that(false && "chain error");
                dump(std::cerr);
                return false;
            }
            // check values are in order
            node_type *next = n->next[l];
            if (n != head && next != tail)
            {
                if (!(less(n->value, next->value)))
                {
                    assert_that(false && "value order error");
                    dump(std::cerr);
                    return false;
                }
            }
            if (n != head)
                ++count;
            n = next;
        }

        if (l == 0 && count != item_count)
        {
            assert_that(false && "item count error")
            dump(std::cerr);
            return false;
        }
    }
    return true;
}
#endif

} // namespace detail
} // namespace goodliffe

//==============================================================================

#ifdef _MSC_VER
#pragma warning( pop )
#endif
