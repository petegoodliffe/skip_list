//==============================================================================
// skip_list.h
// Copyright (c) 2011 Pete Goodliffe. All rights reserved.
//==============================================================================

#pragma once

#include <memory>     // for std::allocator
#include <functional> // for std::less
#include <iterator>   // for std::reverse_iterator
#include <utility>    // for std::pair

//==============================================================================
#pragma mark - detail

namespace goodliffe {

/// @internal
/// Internal namespace for impementation of skip list data structure
namespace detail
{
    template <typename T,
              typename Compare   = std::less<T>,
              typename Allocator = std::allocator<T> >
    class skip_list_impl;
}

//==============================================================================
#pragma mark - skip_list

/// An STL-style skip list container; a reasonably fast ordered container.
///
/// The skip list provides fast searching, and good insert/erase performance.
/// You can iterate bi-directionally, but do not have full random access.
///
/// TODO:
///     * C++11: noexcept decls
///     * C++11: move operations
///     * possibly optimse prev ptrs (not many, just one)
///     * rationalise requirements on contained type
///     * use the Compare
///     * all not_implemented_yet methods
///     * better random generator
///     * make "multi" list version
///
/// Talk about: efficiency, iterator invalidation.
///
/// Following the freaky STL container names, this might be better named
/// unique_sorted_list or sorted_list, or somesuch.
///
/// @param T         Template type for kind of object held in the container.
/// @param Compare   Template type describing the ordering comparator.
///                  Defaults to useing the less than operator.
/// @param Allocator Template type for memory allocator for the contents of
///                  of the container. Defaults to a standard std::allocator
template <typename T,
          typename Compare   = std::less<T>,
          typename Allocator = std::allocator<T> >
class skip_list
{
private:
    typedef detail::skip_list_impl<T,Compare,Allocator> impl_type;
    typedef typename impl_type::node_type               node_type;

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

    class iterator;
    class const_iterator;
    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

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
    iterator insert(InputIterator first, InputIterator last);

    //C++11iterator insert(const_iterator pos, std::initializer_list<value_type> ilist);
    // C++11 emplace

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

private:
    friend class iterator;
    friend class const_iterator;

    impl_type impl;
};

template <class T, class Compare, class Allocator>
bool operator==(const skip_list<T,Compare,Allocator> &lhs, const skip_list<T,Compare,Allocator> &rhs);

template <class T, class Compare, class Allocator>
bool operator!=(const skip_list<T,Compare,Allocator> &lhs, const skip_list<T,Compare,Allocator> &rhs);

template <class T, class Compare, class Allocator>
bool operator<(const skip_list<T,Compare,Allocator> &lhs, const skip_list<T,Compare,Allocator> &rhs);

template <class T, class Compare, class Allocator>
bool operator<=(const skip_list<T,Compare,Allocator> &lhs, const skip_list<T,Compare,Allocator> &rhs);

template <class T, class Compare, class Allocator>
bool operator>(const skip_list<T,Compare,Allocator> &lhs, const skip_list<T,Compare,Allocator> &rhs);

template <class T, class Compare, class Allocator>
bool operator>=(const skip_list<T,Compare,Allocator> &lhs, const skip_list<T,Compare,Allocator> &rhs);
    
} // namespace goodliffe

namespace std
{
    template <class T, class Compare, class Allocator>
    void swap(goodliffe::skip_list<T,Compare,Allocator> &lhs, goodliffe::skip_list<T,Compare,Allocator> &rhs);
}

//==============================================================================
#pragma mark - diagnostics

#define SKIP_LIST_DIAGNOSTICS 1

#ifdef SKIP_LIST_DIAGNOSTICS

void pg_assertion_break();
inline
void pg_assertion_break() { fprintf(stderr, "BREAK HERE\n"); }
#include <cassert>
#include <stdio.h>
#define pg_fail(a)            {fprintf(stderr,"%s:%d: \"%s\"\n", __FILE__, __LINE__, a); /*assert(false);*/ pg_assertion_break(); }
#define assert_that(a)        {if (!(a)) pg_fail(#a);}
#define not_implemented_yet() pg_fail("not implemented yet")

#else

#define pg_fail(a)            
#define assert_that(a)        
#define not_implemented_yet() 

#endif

//==============================================================================
#pragma mark - skip_list_impl declaration

namespace goodliffe {
namespace detail {
        
/// Internal implementation of skip_list data structure and methods for
/// modifying it.
template <typename T, typename Compare, typename Allocator>
class skip_list_impl
{
public:
    enum { max_levels = 33 };

    struct node_type
    {
        node_type *next[max_levels];
        node_type *prev;
        T          value;
    };
    
    typedef T                                                     value_type;
    typedef typename Allocator::size_type                         size_type;
    typedef typename Allocator::template rebind<node_type>::other node_allocator;

    skip_list_impl(const Allocator &alloc = Allocator());
    ~skip_list_impl();

    Allocator get_allocator() const;

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
    void             swap(skip_list_impl &other);

    static unsigned  random_level();
    unsigned         new_level();

    template <typename STREAM>
    void dump(STREAM &stream) const;

private:
    skip_list_impl(const skip_list_impl &other);
    skip_list_impl &operator=(const skip_list_impl &other);

    Allocator   alloc;
    unsigned    levels;
    node_type  *head;
    node_type  *tail;
    size_type   item_count;

    static size_type nodes_between(node_type *first, node_type *last)
    {
        size_type count = 1;
        while (first != last) { ++count; first = first->next[0]; }
        return count;
    }
};
    
} // namespace detail
} // namespace goodliffe

//==============================================================================
#pragma mark - iterators

namespace goodliffe {

template <class T, class Compare, class Allocator>
class skip_list<T,Compare,Allocator>::iterator
    : public std::iterator<std::bidirectional_iterator_tag,
                           typename skip_list<T,Compare,Allocator>::value_type,
                           typename skip_list<T,Compare,Allocator>::difference_type,
                           typename skip_list<T,Compare,Allocator>::pointer,
                           typename skip_list<T,Compare,Allocator>::reference>
{
public:
    typedef skip_list<T,Compare,Allocator>              parent_type;
    typedef typename parent_type::const_iterator        const_type;
    typedef typename parent_type::impl_type::node_type  node_type;
    typedef iterator                                    self_type;
    
    iterator()
        : parent(0), node(0) {}
    iterator(parent_type *parent_, node_type *node_)
        : parent(parent_), node(node_) {}

    self_type &operator++()
        { node = node->next[0]; return *this; }
    self_type operator++(int) // postincrement
        { self_type old(*this); operator++(); return old; }

    self_type &operator--()
        { node = node->prev; return *this; }
    self_type operator--(int) // postdecrement
        { self_type old(*this); operator--(); return old; }
    
    //size_type operator-(const self_type &other) const
    //    { return index - other.index; }
    
    reference operator*()  { return node->value; }
    pointer   operator->() { return node->value; }
    
    bool operator==(const self_type &other) const
        { return parent == other.parent && node == other.node; }
    bool operator!=(const self_type &other) const
        { return !operator==(other); }
    
    bool operator==(const const_type &other) const
        { return parent == other.parent && node == other.node; }
    bool operator!=(const const_type &other) const
        { return !operator==(other); }

    // diagnostic
    const parent_type *get_parent() const { return parent; }
    const node_type   *get_node()   const { return node; }

private:
    friend class const_iterator;
    parent_type *parent;
    node_type   *node;
};

template <class T, class Compare, class Allocator>
class skip_list<T,Compare,Allocator>::const_iterator
    : public std::iterator<std::bidirectional_iterator_tag,
                           typename skip_list<T,Compare,Allocator>::value_type,
                           typename skip_list<T,Compare,Allocator>::difference_type,
                           typename skip_list<T,Compare,Allocator>::const_pointer,
                           typename skip_list<T,Compare,Allocator>::const_reference>
{
public:
    typedef const skip_list<T,Compare,Allocator>                parent_type;
    typedef typename parent_type::iterator                      non_const_type;
    typedef const typename parent_type::impl_type::node_type    node_type;
    typedef const_iterator                                      self_type;

    const_iterator()
        : parent(0), node(0) {}
    const_iterator(const non_const_type &i)
        : parent(i.parent), node(i.node) {}
    const_iterator(const parent_type *parent_, node_type *node_)
        : parent(parent_), node(node_) {}

    self_type &operator++()
        { node = node->next[0]; return *this; }
    self_type operator++(int) // postincrement
        { self_type old(*this); operator++(); return old; }

    self_type &operator--()
        { node = node->prev; return *this; }
    self_type operator--(int) // postdecrement
        { self_type old(*this); operator--(); return old; }
    
    //size_type operator-(const self_type &other) const
    //    { return index - other.index; }
    
    const_reference operator*()  { return node->value; }
    const_pointer   operator->() { return node->value; }
    
    bool operator==(const self_type &other) const
        { return parent == other.parent && node == other.node; }
    bool operator!=(const self_type &other) const
        { return !operator==(other); }

    bool operator==(const non_const_type &other) const
        { return parent == other.parent && node == other.node; }
    bool operator!=(const non_const_type &other) const
        { return !operator==(other); }
    
    // diagnostic
    const parent_type *get_parent() const { return parent; }
    const node_type   *get_node()   const { return node; }

private:
    friend class iterator;
    parent_type *parent;
    node_type   *node;
};

//==============================================================================
#pragma mark - lifetime management

template <class T, class Compare, class Allocator>
inline
skip_list<T,Compare,Allocator>::skip_list(const Allocator &alloc_)
:   impl(alloc_)
{
}

template <class T, class Compare, class Allocator>
inline
skip_list<T,Compare,Allocator>::~skip_list()
{
}

template <class T, class Compare, class Allocator>
template <class InputIterator>
inline
skip_list<T,Compare,Allocator>::skip_list(InputIterator first, InputIterator last, const Allocator &alloc_)
:   impl(alloc_)
{
    assign(first, last);
}

template <class T, class Compare, class Allocator>
inline
skip_list<T,Compare,Allocator>::skip_list(const skip_list &other)
:   impl(other.get_allocator())
{    
    assign(other.begin(), other.end());
}

template <class T, class Compare, class Allocator>
inline
skip_list<T,Compare,Allocator>::skip_list(const skip_list &other, const Allocator &alloc_)
:   impl(alloc_)
{
    assign(other.begin(), other.end());
}

// C++11
//skip_list(const skip_list &&other);
//skip_list(const skip_list &&other, const Allocator &alloc);
//skip_list(std::initializer_list<T> init, const Allocator &alloc = Allocator());

template <class T, class Compare, class Allocator>
inline
Allocator skip_list<T,Compare,Allocator>::get_allocator() const
{
    return impl.get_allocator();
}

//==============================================================================
#pragma mark - assignment

template <class T, class Compare, class Allocator>
inline
skip_list<T,Compare,Allocator> &skip_list<T,Compare,Allocator>::operator=(const skip_list<T,Compare,Allocator> &other)
{
    assign(other.begin(), other.end());
    return *this;
}

//C++11 skip_list& operator=(skip_list&& other);

template <class T, class Compare, class Allocator>
template <typename InputIterator>
inline
void skip_list<T,Compare,Allocator>::assign(InputIterator first, InputIterator last)
{
    clear();
    while (first != last) insert(*first++);
}

//==============================================================================
#pragma mark - element access

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::reference
skip_list<T,Compare,Allocator>::front()
{
    assert_that(!empty());
    return impl.front()->value;
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_reference
skip_list<T,Compare,Allocator>::front() const
{
    assert_that(!empty());
    return impl.front()->value;
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::reference
skip_list<T,Compare,Allocator>::back()
{
    assert_that(!empty());
    return impl.one_past_end()->prev->value;
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_reference
skip_list<T,Compare,Allocator>::back() const
{
    assert_that(!empty());
    return impl.one_past_end()->prev->value;
}

//==============================================================================
#pragma mark - iterators

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::iterator
skip_list<T,Compare,Allocator>::begin()
{
    return iterator(this, impl.front());
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_iterator
skip_list<T,Compare,Allocator>::begin() const
{
    return const_iterator(this, impl.front());
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_iterator
skip_list<T,Compare,Allocator>::cbegin() const
{
    return const_iterator(this, impl.front());
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::iterator
skip_list<T,Compare,Allocator>::end()
{
    return iterator(this, impl.one_past_end());
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_iterator
skip_list<T,Compare,Allocator>::end() const
{
    return const_iterator(this, impl.one_past_end());
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_iterator
skip_list<T,Compare,Allocator>::cend() const
{
    return const_iterator(this, impl.one_past_end());
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::reverse_iterator
skip_list<T,Compare,Allocator>::rbegin()
{
    return reverse_iterator(end());
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_reverse_iterator
skip_list<T,Compare,Allocator>::rbegin() const
{
    return const_reverse_iterator(end());
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_reverse_iterator
skip_list<T,Compare,Allocator>::crbegin() const
{
    return const_reverse_iterator(end());
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::reverse_iterator
skip_list<T,Compare,Allocator>::rend()
{
    return reverse_iterator(begin());
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_reverse_iterator
skip_list<T,Compare,Allocator>::rend() const
{
    return const_reverse_iterator(begin());
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_reverse_iterator
skip_list<T,Compare,Allocator>::crend() const
{
    return const_reverse_iterator(begin());
}

//==============================================================================
#pragma mark - capacity

template <class T, class Compare, class Allocator>
inline
bool skip_list<T,Compare,Allocator>::empty() const
{
    return impl.size() == 0;
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::size_type
skip_list<T,Compare,Allocator>::size() const
{
    return impl.size();
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::size_type
skip_list<T,Compare,Allocator>::max_size() const
{
    return impl.get_allocator().max_size();
}

//==============================================================================
#pragma mark - modifiers

template <class T, class Compare, class Allocator>
inline
void skip_list<T,Compare,Allocator>::clear()
{
    impl.remove_all();
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::insert_by_value_result
skip_list<T,Compare,Allocator>::insert(const value_type &value)
{
    node_type *node = impl.insert(value);
    return std::make_pair(iterator(this, node), impl.is_valid(node));
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::iterator
skip_list<T,Compare,Allocator>::insert(const_iterator hint, const value_type &value)
{
    assert_that(hint.get_parent() == this);
    
    const node_type *hint_node = hint.get_node();
    const node_type *previous  = hint_node->prev;
    if (impl.is_valid(previous))
    {
        if (previous->value > value)
        {
            // bad hint, resort to "normal" insert
            return iterator(this,impl.insert(value));
        }
    }
    else
    {
        hint_node = 0;
    }

    return iterator(this,impl.insert(value,const_cast<node_type*>(hint_node)));
}

//C++11iterator insert const_iterator pos, value_type &&value);

template <class T, class Compare, class Allocator>
template <class InputIterator>
inline
typename skip_list<T,Compare,Allocator>::iterator
skip_list<T,Compare,Allocator>::insert(InputIterator first, InputIterator last)
{
    iterator last_inserted = end();
    while (first != last)
    {
        if (last_inserted != end() && *first < *last_inserted)
        {
            last_inserted = end();
        }
        last_inserted = insert(last_inserted, *first++);
    }
    
    // return one-beyond the last inserted item
    if (last_inserted != end())
        ++last_inserted;

    return last_inserted;
}

//C++11iterator insert(const_iterator pos, std::initializer_list<value_type> ilist);
// C++11 emplace

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::size_type
skip_list<T,Compare,Allocator>::erase(const value_type &value)
{
    node_type *node = impl.find(value);
    if (impl.is_valid(node) && node->value == value)
    {
        impl.remove(node);
        return 1;
    }
    else
    {
        return 0;
    }
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::iterator
skip_list<T,Compare,Allocator>::erase(const_iterator position)
{
    assert_that(position.get_parent() == this);
    assert_that(impl.is_valid(position.get_node()));
    node_type *node = const_cast<node_type*>(position.get_node());
    node_type *next = node->next[0];
    impl.remove(node);
    return iterator(this, next);
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::iterator
skip_list<T,Compare,Allocator>::erase(const_iterator first, const_iterator last)
{
    assert_that(first.get_parent() == this);
    assert_that(last.get_parent() == this);

    if (first != last)
    {
        node_type *first_node = const_cast<node_type*>(first.get_node());
        node_type *last_node  = const_cast<node_type*>(last.get_node()->prev);
        impl.remove_between(first_node, last_node);
    }
    
    return iterator(this, const_cast<node_type*>(last.get_node()));
}

template <class T, class Compare, class Allocator>
inline
void
skip_list<T,Compare,Allocator>::swap(skip_list<T,Compare,Allocator> &other)
{
    impl.swap(other.impl);
}
    
//==============================================================================
#pragma mark - lookup

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::size_type
skip_list<T,Compare,Allocator>::count(const value_type &value) const
{
    const node_type *node = impl.find(value);
    return impl.is_valid(node) && node->value == value;
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::iterator
skip_list<T,Compare,Allocator>::find(const value_type &value)
{
    node_type *node = impl.find(value);
    return impl.is_valid(node) && node->value == value
        ? iterator(this, node)
        : end();
}
  
template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_iterator
skip_list<T,Compare,Allocator>::find(const value_type &value) const
{
    const node_type *node = impl.find(value);
    return impl.is_valid(node) && node->value == value
        ? const_iterator(this, node)
        : end();
}

//==============================================================================
#pragma mark - non-members

template <class T, class Compare, class Allocator>
inline
bool operator==(const skip_list<T,Compare,Allocator> &lhs, const skip_list<T,Compare,Allocator> &rhs)
{
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T, class Compare, class Allocator>
inline
bool operator!=(const skip_list<T,Compare,Allocator> &lhs, const skip_list<T,Compare,Allocator> &rhs)
{
    return !operator==(lhs, rhs);
}

template <class T, class Compare, class Allocator>
inline
bool operator<(const skip_list<T,Compare,Allocator> &lhs, const skip_list<T,Compare,Allocator> &rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <class T, class Compare, class Allocator>
inline
bool operator<=(const skip_list<T,Compare,Allocator> &lhs, const skip_list<T,Compare,Allocator> &rhs)
{
    return !(rhs < lhs);
}

template <class T, class Compare, class Allocator>
inline
bool operator>(const skip_list<T,Compare,Allocator> &lhs, const skip_list<T,Compare,Allocator> &rhs)
{
    return rhs < lhs;
}

template <class T, class Compare, class Allocator>
inline
bool operator>=(const skip_list<T,Compare,Allocator> &lhs, const skip_list<T,Compare,Allocator> &rhs)
{
    return !(lhs < rhs);
}
    
} // namespace goodliffe

template <class T, class Compare, class Allocator>
inline
void std::swap(goodliffe::skip_list<T,Compare,Allocator> &lhs, goodliffe::skip_list<T,Compare,Allocator> &rhs)
{
    lhs.swap(rhs);
}

//==============================================================================
#pragma mark - skip_list_impl

namespace goodliffe {
namespace detail {

template <class T, class Compare, class Allocator>
inline
skip_list_impl<T,Compare,Allocator>::skip_list_impl(const Allocator &alloc_)
:   alloc(alloc_),
    levels(0),
    head(node_allocator(alloc).allocate(1, (void*)0)),
    tail(node_allocator(alloc).allocate(1, (void*)0)),
    item_count(0)
{
    for (unsigned n = 0; n < max_levels; n++)
    {
        head->next[n] = tail;
        tail->next[n] = 0;
    }
    head->prev = 0;
    tail->prev = head;
}

template <class T, class Compare, class Allocator>
inline
skip_list_impl<T,Compare,Allocator>::~skip_list_impl()
{
    remove_all();
    node_allocator(alloc).deallocate(head, 1);
    node_allocator(alloc).deallocate(tail, 1);
}

template <class T, class Compare, class Allocator>
inline
Allocator skip_list_impl<T,Compare,Allocator>::get_allocator() const
{
    return alloc;
}

template <class T, class Compare, class Allocator>
inline
typename skip_list_impl<T,Compare,Allocator>::node_type *
skip_list_impl<T,Compare,Allocator>::find(const value_type &value) const
{
    // TODO: == <=
    // I could have a const and non-const overload, but this is simpler
    node_type *search = const_cast<node_type*>(head);
    for (unsigned l = levels; l; )
    {
        --l;
        while (search->next[l] != tail && search->next[l]->value <= value)
            search = search->next[l];
    }
    return search;
}

template <class T, class Compare, class Allocator>
inline
typename skip_list_impl<T,Compare,Allocator>::node_type*
skip_list_impl<T,Compare,Allocator>::insert(const value_type &value, node_type *hint)
{
    const unsigned level = new_level();

    node_type *new_node = node_allocator(alloc).allocate(1, (void*)0);
    alloc.construct(&new_node->value, value);
    
    // TODO: This probably doesn't matter
    {
        for (unsigned n = 0; n < max_levels; ++n)
            new_node->next[n] = tail;
        new_node->prev = head;
    }

    node_type *insert_point = hint ? hint : head;
    for (unsigned l = levels; l; )
    {
        --l;
        while (insert_point->next[l] != tail && insert_point->next[l]->value < value)
            insert_point = insert_point->next[l];
        
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
    node_type *next = new_node->next[0];
    assert_that(next);
    new_node->prev = insert_point;
    next->prev = new_node;

    ++item_count;

    if (next != tail && next->value == value)
    {
        remove(new_node);
        new_node = tail;
    }
    
    return new_node;
}

template <class T, class Compare, class Allocator>
inline
void
skip_list_impl<T,Compare,Allocator>::remove(node_type *node)
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
        while (cur->next[l] != tail && cur->next[l]->value <= node->value)
        {
            if (cur->next[l] == node)
            {
                cur->next[l] = node->next[l];
                break;
            }
            cur = cur->next[l];
        }
    }

    alloc.destroy(&node->value);
    node_allocator(alloc).deallocate(node, 1u);

    item_count--;
}

template <class T, class Compare, class Allocator>
inline
void
skip_list_impl<T,Compare,Allocator>::remove_all()
{
    node_type *node = head->next[0];
    while (node != tail)
    {
        node_type *next = node->next[0];
        alloc.destroy(&node->value);
        node_allocator(alloc).deallocate(node, 1u);
        node = next;
    }

    for (unsigned l = 0; l < max_levels; ++l)
        head->next[l] = tail;
    tail->prev = head;
    item_count = 0;
}

template <class T, class Compare, class Allocator>
inline
void 
skip_list_impl<T,Compare,Allocator>::remove_between(node_type *first, node_type *last)
{
    assert_that(first != head);
    assert_that(first != tail);
    assert_that(last != head);
    assert_that(last != tail);

    // TODO: put back faster version
    last = last->next[0];
    while (first != last)
    {
        node_type *next = first->next[0];
        remove(first);
        first = next;
    }
    
    /*
    for (unsigned l = 0; l < max_levels; ++l)
    {
        node_type *next = last->next[l];
        assert_that(next);

        prev->next[l] = next;
        next->prev[l] = prev;
    }
    // TODO
    node_type *prev = first->prev;
    assert_that(prev);
    
    //size_type distance = nodes_between(first, last);
    //item_count -= distance;
    
    const node_type *one_past_end = last->next[0];
    while (first != one_past_end)
    {
        node_type *next = first->next[0];
        alloc.destroy(&first->value);
        node_allocator(alloc).deallocate(first, 1u);
        item_count--;
        first = next;
    }*/
}

/// Generate a stream of levels, probabilstically chosen.
/// With a probability of 1/2, return 0.
/// With 1/4 probability, return 1.
/// With 1/8 probability, return 2.
/// And so forth.
/// TODO: Consider making this a template method
template <class T, class Compare, class Allocator>
inline
unsigned skip_list_impl<T,Compare,Allocator>::random_level()
{
    // The number of 1-bits before we encounter the first 0-bit is the level of
    /// the node. Since R is 32-bit, the level can be at most 32.
    unsigned level = 0;
    for (unsigned number = unsigned(rand()); (number & 1) == 1; number >>= 1)
    {
        level++;
    }
    return level;
}

template <class T, class Compare, class Allocator>
inline
unsigned skip_list_impl<T,Compare,Allocator>::new_level()
{    
    unsigned level = random_level();
    if (level >= levels)
    {
        level = levels;
        ++levels;
    }
    return level;
}

template <class T, class Compare, class Allocator>
inline
void skip_list_impl<T,Compare,Allocator>::swap(skip_list_impl &other)
{
    using std::swap;

    swap(alloc,      other.alloc); // might throw? others shouldn't
    swap(levels,     other.levels);
    swap(head,       other.head);
    swap(tail,       other.tail);
    swap(item_count, other.item_count);
}

// for diagnostics only
template <class T, class Compare, class Allocator>
template <class STREAM>
inline
void skip_list_impl<T,Compare,Allocator>::dump(STREAM &s) const
{
    s << "skip_list(size="<<item_count<<",levels=" << levels << ")\n";
    for (unsigned l = 0; l < levels; ++l)
    {
        s << "  [" << l << "] ";
        const node_type *n = head;
        while (n)
        {
            const node_type *next = n->next[l];
            bool prev_ok = false;
            if (next)
            {
                if (next->prev == n) prev_ok = true;
            }
            if (is_valid(n))
                s << n->value;
            else
                s << "*";
            
            if (n != tail)
            {
                s << (next ? ">":"|")
                  << " "
                  << (prev_ok?"<":"X");
            }
            n = next;
        }
        s << "\n";
    }
}

} // namespace detail
} // namespace goodliffe
