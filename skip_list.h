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
#pragma mark - internal forward declarations

namespace goodliffe {

/// @internal
/// Internal namespace for impementation of skip list data structure
namespace detail
{
    template <typename T,typename C,typename A,typename LG>
    class sl_impl;

    template <typename LIST> class sl_iterator;
    template <typename LIST> class sl_const_iterator;
}
}

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
/// For full random access, use the skip_list container.
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
          typename LevelGenerator = detail::skip_list_level_generator<32> >
class skip_list
{
protected:
    typedef typename detail::sl_impl<T,Compare,Allocator,LevelGenerator> impl_type;
    typedef typename impl_type::node_type                                node_type;

    template <typename T1> friend class detail::sl_iterator;
    template <typename T1> friend class detail::sl_const_iterator;

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
    
    typedef typename detail::sl_iterator<impl_type>     iterator;
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

    allocator_type get_allocator() const { return impl.get_allocator(); }

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

    iterator       begin()                  { return iterator(&impl, impl.front()); }
    const_iterator begin() const            { return const_iterator(&impl, impl.front()); }
    const_iterator cbegin() const           { return const_iterator(&impl, impl.front()); }

    iterator       end()                    { return iterator(&impl, impl.one_past_end()); }
    const_iterator end() const              { return const_iterator(&impl, impl.one_past_end()); }
    const_iterator cend() const             { return const_iterator(&impl, impl.one_past_end()); }

    reverse_iterator       rbegin()         { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const   { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const  { return const_reverse_iterator(end()); }

    reverse_iterator       rend()           { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const     { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const    { return const_reverse_iterator(begin()); }

    //======================================================================
    // capacity

    bool      empty() const         { return impl.size() == 0; }
    size_type size() const          { return impl.size(); }
    size_type max_size() const      { return impl.get_allocator().max_size(); }

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

    void swap(skip_list &other) { impl.swap(other.impl); }

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

} // namespace goodliffe

//==============================================================================
#pragma mark - non-members

namespace goodliffe {

template <class T, class C, class A, class LG>
inline
bool operator==(const skip_list<T,C,A,LG> &lhs, const skip_list<T,C,A,LG> &rhs)
{
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T, class C, class A, class LG>
inline
bool operator!=(const skip_list<T,C,A,LG> &lhs, const skip_list<T,C,A,LG> &rhs)
{
    return !operator==(lhs, rhs);
}

template <class T, class C, class A, class LG>
inline
bool operator<(const skip_list<T,C,A,LG> &lhs, const skip_list<T,C,A,LG> &rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <class T, class C, class A, class LG>
inline
bool operator<=(const skip_list<T,C,A,LG> &lhs, const skip_list<T,C,A,LG> &rhs)
{
    return !(rhs < lhs);
}

template <class T, class C, class A, class LG>
inline
bool operator>(const skip_list<T,C,A,LG> &lhs, const skip_list<T,C,A,LG> &rhs)
{
    return rhs < lhs;
}

template <class T, class C, class A, class LG>
inline
bool operator>=(const skip_list<T,C,A,LG> &lhs, const skip_list<T,C,A,LG> &rhs)
{
    return !(lhs < rhs);
}

} // namespace goodliffe

namespace std
{
    template <class T, class C, class A, class LG>
    void swap(goodliffe::skip_list<T,C,A,LG> &lhs, goodliffe::skip_list<T,C,A,LG> &rhs)
    {
        lhs.swap(rhs);
    }
}

//==============================================================================
#pragma mark - iterators
//==============================================================================

namespace goodliffe {
namespace detail {

template <typename SL_IMPL>
class sl_iterator
    : public std::iterator<std::bidirectional_iterator_tag,
                           typename SL_IMPL::value_type,
                           typename SL_IMPL::difference_type,
                           typename SL_IMPL::const_pointer,
                           typename SL_IMPL::const_reference>
{
public:
    typedef SL_IMPL                         impl_type;
    typedef sl_const_iterator<impl_type>    const_iterator;
    typedef typename impl_type::node_type   node_type;
    typedef sl_iterator<impl_type>          self_type;

    typedef typename impl_type::const_reference const_reference;
    typedef typename impl_type::const_pointer   const_pointer;

    sl_iterator() :
#ifdef SKIP_LIST_DIAGNOSTICS
        impl(0),
#endif
        node(0) {}
    sl_iterator(impl_type *impl_, node_type *node_) :
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

    const_reference operator*()  { return node->value; }
    const_pointer   operator->() { return node->value; }
    
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

template <class SL_IMPL>
class sl_const_iterator
    : public std::iterator<std::bidirectional_iterator_tag,
                           typename SL_IMPL::value_type,
                           typename SL_IMPL::difference_type,
                           typename SL_IMPL::const_pointer,
                           typename SL_IMPL::const_reference>
{
public:
    typedef const SL_IMPL                       impl_type;
    typedef sl_iterator<SL_IMPL>                iterator;
    typedef const typename impl_type::node_type node_type;
    typedef sl_const_iterator<SL_IMPL>          self_type;

    typedef typename impl_type::const_reference const_reference;
    typedef typename impl_type::const_pointer   const_pointer;

    sl_const_iterator() :
#ifdef SKIP_LIST_DIAGNOSTICS
        impl(0),
#endif
        node(0) {}
    sl_const_iterator(const iterator &i) :
#ifdef SKIP_LIST_DIAGNOSTICS
        impl(i.get_impl()),
#endif
        node(i.get_node()) {}
    sl_const_iterator(const impl_type *impl_, node_type *node_) :
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

    const_reference operator*()  { return node->value; }
    const_pointer   operator->() { return node->value; }

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

template <class T, class C, class A, class LG>
inline
skip_list<T,C,A,LG>::skip_list(const allocator_type &alloc_)
:   impl(alloc_)
{
}

template <class T, class C, class A, class LG>
inline
skip_list<T,C,A,LG>::~skip_list()
{
}

template <class T, class C, class A, class LG>
template <class InputIterator>
inline
skip_list<T,C,A,LG>::skip_list(InputIterator first, InputIterator last, const allocator_type &alloc_)
:   impl(alloc_)
{
    assign(first, last);
}

template <class T, class C, class A, class LG>
inline
skip_list<T,C,A,LG>::skip_list(const skip_list &other)
:   impl(other.get_allocator())
{    
    assign(other.begin(), other.end());
}

template <class T, class C, class A, class LG>
inline
skip_list<T,C,A,LG>::skip_list(const skip_list &other, const allocator_type &alloc_)
:   impl(alloc_)
{
    assign(other.begin(), other.end());
}

// C++11
//skip_list(const skip_list &&other);
//skip_list(const skip_list &&other, const Allocator &alloc);
//skip_list(std::initializer_list<T> init, const Allocator &alloc = Allocator());

//==============================================================================
#pragma mark assignment

template <class T, class C, class A, class LG>
inline
skip_list<T,C,A,LG> &
skip_list<T,C,A,LG>::operator=(const skip_list<T,C,A,LG> &other)
{
    assign(other.begin(), other.end());
    return *this;
}

//C++11 skip_list& operator=(skip_list&& other);

template <class T, class C, class A, class LG>
template <typename InputIterator>
inline
void skip_list<T,C,A,LG>::assign(InputIterator first, InputIterator last)
{
    clear();
    while (first != last) insert(*first++);
}

//==============================================================================
#pragma mark element access

template <class T, class C, class A, class LG>
inline
typename skip_list<T,C,A,LG>::reference
skip_list<T,C,A,LG>::front()
{
    assert_that(!empty());
    return impl.front()->value;
}

template <class T, class C, class A, class LG>
inline
typename skip_list<T,C,A,LG>::const_reference
skip_list<T,C,A,LG>::front() const
{
    assert_that(!empty());
    return impl.front()->value;
}

template <class T, class C, class A, class LG>
inline
typename skip_list<T,C,A,LG>::reference
skip_list<T,C,A,LG>::back()
{
    assert_that(!empty());
    return impl.one_past_end()->prev->value;
}

template <class T, class C, class A, class LG>
inline
typename skip_list<T,C,A,LG>::const_reference
skip_list<T,C,A,LG>::back() const
{
    assert_that(!empty());
    return impl.one_past_end()->prev->value;
}

//==============================================================================
#pragma mark modifiers

template <class T, class C, class A, class LG>
inline
void skip_list<T,C,A,LG>::clear()
{
    impl.remove_all();
}

template <class T, class C, class A, class LG>
inline
typename skip_list<T,C,A,LG>::insert_by_value_result
skip_list<T,C,A,LG>::insert(const value_type &value)
{
    node_type *node = impl.insert(value);
    return std::make_pair(iterator(&impl, node), impl.is_valid(node));
}

template <class T, class C, class A, class LG>
inline
typename skip_list<T,C,A,LG>::iterator
skip_list<T,C,A,LG>::insert(const_iterator hint, const value_type &value)
{
    assert_that(hint.get_impl() == &impl);
    
    const node_type *hint_node = hint.get_node();

    if (impl.is_valid(hint_node) && detail::less_or_equal(value, hint_node->value, impl.less))
        return iterator(&impl,impl.insert(value)); // bad hint, resort to "normal" insert
    else
        return iterator(&impl,impl.insert(value,const_cast<node_type*>(hint_node)));
}

//C++11iterator insert const_iterator pos, value_type &&value);

template <class T, class C, class A, class LG>
template <class InputIterator>
inline
void
skip_list<T,C,A,LG>::insert(InputIterator first, InputIterator last)
{
    iterator last_inserted = end();
    while (first != last)
    {
        last_inserted = insert(last_inserted, *first++);
    }
}

//C++11iterator insert(std::initializer_list<value_type> ilist);
// C++11 emplace

template <class T, class C, class A, class LG>
inline
typename skip_list<T,C,A,LG>::size_type
skip_list<T,C,A,LG>::erase(const value_type &value)
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

template <class T, class C, class A, class LG>
inline
typename skip_list<T,C,A,LG>::iterator
skip_list<T,C,A,LG>::erase(const_iterator position)
{
    assert_that(position.get_impl() == &impl);
    assert_that(impl.is_valid(position.get_node()));
    node_type *node = const_cast<node_type*>(position.get_node());
    node_type *next = node->next[0];
    impl.remove(node);
    return iterator(&impl, next);
}

template <class T, class C, class A, class LG>
inline
typename skip_list<T,C,A,LG>::iterator
skip_list<T,C,A,LG>::erase(const_iterator first, const_iterator last)
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
  
//==============================================================================
#pragma mark lookup

template <class T, class C, class A, class LG>
inline
typename skip_list<T,C,A,LG>::size_type
skip_list<T,C,A,LG>::count(const value_type &value) const
{
    const node_type *node = impl.find(value);
    return impl.is_valid(node) && detail::equivalent(node->value, value, impl.less);
}

template <class T, class C, class A, class LG>
inline
typename skip_list<T,C,A,LG>::iterator
skip_list<T,C,A,LG>::find(const value_type &value)
{
    node_type *node = impl.find(value);
    return impl.is_valid(node) && detail::equivalent(node->value, value, impl.less)
        ? iterator(&impl, node)
        : end();
}
  
template <class T, class C, class A, class LG>
inline
typename skip_list<T,C,A,LG>::const_iterator
skip_list<T,C,A,LG>::find(const value_type &value) const
{
    const node_type *node = impl.find(value);
    return impl.is_valid(node) && detail::equivalent(node->value, value, impl.less)
        ? const_iterator(&impl, node)
        : end();
}
    
} // namespace goodliffe

//==============================================================================
#pragma mark - sl_impl
//==============================================================================

namespace goodliffe {
namespace detail {

template <typename T>
struct sl_node
{
    typedef sl_node<T> self_type;

#ifdef sl_impl_DIAGNOSTICS
    unsigned    magic;
#endif
    T           value;
    unsigned    level;
    self_type  *prev;
    self_type **next; ///< effectively node_type *next[level+1];
};

/// Internal implementation of skip_list data structure and methods for
/// modifying it.
///
/// Not for "public" access.
///
/// @internal
template <typename T, typename Compare, typename Allocator, typename LevelGenerator>
class sl_impl
{
public:
    
    typedef T                                   value_type;
    typedef typename Allocator::size_type       size_type;
    typedef typename Allocator::difference_type difference_type;
    typedef typename Allocator::const_reference const_reference;
    typedef typename Allocator::const_pointer   const_pointer;
    typedef Allocator                           allocator_type;
    typedef Compare                             compare_type;
    typedef LevelGenerator                      generator_type;
    typedef sl_node<T>                          node_type;

    static const unsigned num_levels = LevelGenerator::num_levels;

    sl_impl(const Allocator &alloc = Allocator());
    ~sl_impl();

    Allocator        get_allocator() const                 { return alloc; }
    size_type        size() const                          { return item_count; }
    bool             is_valid(const node_type *node) const { return node && node != head && node != tail; }
    node_type       *front()                               { return head->next[0]; }
    const node_type *front() const                         { return head->next[0]; }
    node_type       *one_past_end()                        { return tail; }
    const node_type *one_past_end() const                  { return tail; }
    node_type       *find(const value_type &value) const;
    node_type       *insert(const value_type &value, node_type *hint = 0);
    void             remove(node_type *value);
    void             remove_all();
    void             remove_between(node_type *first, node_type *last);
    void             swap(sl_impl &other);

    template <typename STREAM>
    void        dump(STREAM &stream) const;
    bool        check() const;
    unsigned    new_level();

    compare_type less;

private:
    typedef typename Allocator::template rebind<node_type>::other    node_allocator;
    typedef typename Allocator::template rebind<node_type*>::other   list_allocator;

    sl_impl(const sl_impl &other);
    sl_impl &operator=(const sl_impl &other);
    
    allocator_type  alloc;
    generator_type  generator;
    unsigned        levels;
    node_type      *head;
    node_type      *tail;
    size_type       item_count;
    
    node_type *allocate(unsigned level)
    {
        node_type *node = node_allocator(alloc).allocate(1, (void*)0);
        node->next  = list_allocator(alloc).allocate(level+1, (void*)0);
        node->level = level;
#ifdef sl_impl_DIAGNOSTICS
        for (unsigned n = 0; n <= level; ++n) node->next[n] = 0;
        node->magic = MAGIC_GOOD;
#endif
        return node;
    }

    void deallocate(node_type *node)
    {
#ifdef sl_impl_DIAGNOSTICS
        assert_that(node->magic == MAGIC_GOOD);
        node->magic = MAGIC_BAD;
        for (unsigned n = 0; n <= node->level; ++n) node->next[n] = 0;
        node->prev = 0;
#endif
        list_allocator(alloc).deallocate(node->next, node->level+1);
        node_allocator(alloc).deallocate(node, 1);
    }
};

template <class T, class C, class A, class LG>
inline
sl_impl<T,C,A,LG>::sl_impl(const allocator_type &alloc_)
:   alloc(alloc_),
    levels(0),
    head(allocate(num_levels)),
    tail(allocate(num_levels)),
    item_count(0)
{
    for (unsigned n = 0; n < num_levels; n++)
    {
        head->next[n] = tail;
        tail->next[n] = 0;
    }
    head->prev = 0;
    tail->prev = head;
}

template <class T, class C, class A, class LG>
inline
sl_impl<T,C,A,LG>::~sl_impl()
{
    remove_all();
    deallocate(head);
    deallocate(tail);
}

template <class T, class C, class A, class LG>
inline
typename sl_impl<T,C,A,LG>::node_type *
sl_impl<T,C,A,LG>::find(const value_type &value) const
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

// TODO: Hint is now ignored (has to be, for spans to work)
template <class T, class C, class A, class LG>
inline
typename sl_impl<T,C,A,LG>::node_type*
sl_impl<T,C,A,LG>::insert(const value_type &value, node_type *hint)
{
    const unsigned level = new_level();

    node_type *new_node = allocate(level);
    assert_that(new_node);
    assert_that(new_node->level == level);
    alloc.construct(&new_node->value, value);

    const bool good_hint    = is_valid(hint) && hint->level == levels-1;
    node_type *insert_point = good_hint ? hint : head;
    unsigned   l            = levels;

    while (l)
    {
        --l;
        assert_that(l <= insert_point->level);
        while (insert_point->next[l] != tail && less(insert_point->next[l]->value, value))
        {
            insert_point = insert_point->next[l];
            assert_that(l <= insert_point->level);
        }
        
        if (l <= level)
        {
            node_type *next = insert_point->next[l];
            assert_that(next);
        
            new_node->next[l]     = next;
            insert_point->next[l] = new_node;
        }
    }
    
    // By the time we get here, insert_point is the level 0 node immediately
    // preceding new_node
    assert_that(insert_point->next[0] == new_node);
    node_type *next = new_node->next[0];
    assert_that(next);
    new_node->prev = insert_point;
    next->prev = new_node;

    ++item_count;
          
#if defined SKIP_LIST_IMPL_DIAGNOSTICS
      for (unsigned n = 0; n < level; ++n)
      {
          assert_that(new_node->next[n] != 0);
      }
#endif

    // Do not allow repeated values in the list
    if (next != tail && detail::equivalent(next->value, value, less))
    {
        remove(new_node);
        new_node = tail;
    }

#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    check();
#endif

    return new_node;
}

template <class T, class C, class A, class LG>
inline
void
sl_impl<T,C,A,LG>::remove(node_type *node)
{
    assert_that(node != head);
    assert_that(node != tail);
    assert_that(node->next[0]);

    node->next[0]->prev = node->prev;

    // patch up all next pointers
    node_type *cur = head;
    for (unsigned l = levels; l; )
    {
        --l;
        assert_that(l <= cur->level);
        while (cur->next[l] != tail && less(cur->next[l]->value, node->value))
        {
            cur = cur->next[l];
        }
        if (cur->next[l] == node)
        {
            cur->next[l] = node->next[l];
        }
    }

    alloc.destroy(&node->value);
    deallocate(node);

    item_count--;
    
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    check();
#endif
}

template <class T, class C, class A, class LG>
inline
void
sl_impl<T,C,A,LG>::remove_all()
{
    node_type *node = head->next[0];
    while (node != tail)
    {
        node_type *next = node->next[0];
        alloc.destroy(&node->value);
        deallocate(node);
        node = next;
    }

    for (unsigned l = 0; l < num_levels; ++l)
        head->next[l] = tail;
    tail->prev = head;
    item_count = 0;
        
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    check();
#endif
}

template <class T, class C, class A, class LG>
inline
void 
sl_impl<T,C,A,LG>::remove_between(node_type *first, node_type *last)
{
    assert_that(first != head);
    assert_that(first != tail);
    assert_that(last != head);
    assert_that(last != tail);

    node_type       * const prev         = first->prev;
    node_type       * const one_past_end = last->next[0];
    const value_type       &first_value  = first->value;
    const value_type       &last_value   = last->value;

    // backwards pointer
    one_past_end->prev = prev;    

    // forwards pointers
    node_type *cur = head;
    for (unsigned l = levels; l; )
    {
        --l;
        assert_that(l < cur->level);
        while (cur->next[l] != tail && less(cur->next[l]->value, first_value))
        {
            cur = cur->next[l];
        }
        if (cur->next[l] != tail
            && detail::less_or_equal(cur->next[l]->value, last_value, less))
        {
            // patch up next[l] pointer
            node_type *end = cur->next[l];
            while (end != tail && detail::less_or_equal(end->value, last_value, less))
                end = end->next[l];
            cur->next[l] = end;
        }
    }

    // now delete all the nodes between [first,last]
    while (first != one_past_end)
    {
        node_type *next = first->next[0];
        alloc.destroy(&first->value);
        deallocate(first);
        item_count--;
        first = next;
    }
        
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    check();
#endif
}

template <class T, class C, class A, class LG>
inline
unsigned sl_impl<T,C,A,LG>::new_level()
{    
    unsigned level = generator.new_level();
    if (level >= levels)
    {
        level = levels;
        ++levels;
    }
    return level;
}

template <class T, class C, class A, class LG>
inline
void sl_impl<T,C,A,LG>::swap(sl_impl &other)
{
    using std::swap;

    swap(alloc,      other.alloc);
    swap(less,       other.less);
    swap(generator,  other.generator);
    swap(levels,     other.levels);
    swap(head,       other.head);
    swap(tail,       other.tail);
    swap(item_count, other.item_count);

#ifdef sl_impl_DIAGNOSTICS
    check();
#endif
}

// for diagnostics only
template <class T, class C, class A, class LG>
template <class STREAM>
inline
void sl_impl<T,C,A,LG>::dump(STREAM &s) const
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
            bool prev_ok = false;
            if (next)
            {
                if (next->prev == n) prev_ok = true;
            }
            if (is_valid(n))
                s << n->value << " ";
            else
                s << "* ";
            
            if (n != tail)
            {
                if (next != tail && !less(n->value, next->value))
                    s << "*XXXXXXXXX* ";
                s << ">"
                  << " "
                  << (prev_ok?"<":"X");
            }
            n = next;
        }
        s << "\n";
    }
}

#ifdef sl_impl_DIAGNOSTICS
// for diagnostics only
template <class T, class C, class A, class LG>
inline
bool sl_impl<T,C,A,LG>::check() const
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
