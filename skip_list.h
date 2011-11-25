//==============================================================================
// skip_list.h
// Copyright (c) 2011 Pete Goodliffe. All rights reserved
//==============================================================================

#pragma once

#include <memory>     // for std::allocator
#include <functional> // for std::less
#include <iterator>   // for std::reverse_iterator

//==============================================================================
// diagnostics

#include <cassert>
#include <stdio.h>
#define pg_fail(a)            {fprintf(stderr,"%s:%d: \"%s\"\n", __FILE__, __LINE__, a); assert(false);}
#define assert_that(a)        {if (!a) pg_fail(#a);}
#define not_implemented_yet() pg_fail("not implemented yet")

//==============================================================================

namespace goodliffe {

/// An STL-style skip list container; a reasonably fast ordered container.
///
/// TODO: C++11: noexcept decls
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
public:

    //======================================================================
    // types

    typedef T                                               value_type;
    typedef Allocator                                       allocator_type;
    typedef typename allocator_type::size_type              size_type;
    typedef typename allocator_type::difference_type        difference_type;
    typedef typename allocator_type::reference              reference;
    typedef typename allocator_type::const_reference        const_reference;
    typedef typename allocator_type::pointer                pointer;
    typedef typename allocator_type::const_pointer          const_pointer;
    typedef Compare                                         compare;

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

    void assign(size_type count, const value_type &value);

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

    iterator insert(const_iterator pos, const value_type &value);

    //C++11iterator insert const_iterator pos, value_type &&value);

    iterator insert(const_iterator pos, size_type count, const value_type &value);

    template <class InputIterator>
    iterator insert(const_iterator pos, InputIterator first, InputIterator last);

    //C++11iterator insert(const_iterator pos, std::initializer_list<value_type> ilist);

    // C++11 emplace
    iterator erase(const_iterator position);
    iterator erase(const_iterator first, const_iterator last);

    void push_back(const value_type &value);
    //C++11void push_back( value_type &&value );

    //C++11 emplace_back

    void swap(skip_list &other);

    friend void swap(skip_list &lhs, skip_list &rhs) { lhs.swap(rhs); }

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

private:
    struct node
    {
        node *next;
        value_type value;
    };

    typedef typename allocator_type::template rebind<node>::other node_allocator;

    static const unsigned max_levels = 33;

    allocator_type  alloc;
    unsigned        levels;
    node           *nodes[max_levels];

    unsigned random_level();
};

template <class T, class Compare, class Allocator>
bool operator==(skip_list<T,Compare,Allocator> &lhs, skip_list<T,Compare,Allocator> &rhs);

template <class T, class Compare, class Allocator>
bool operator!=(skip_list<T,Compare,Allocator> &lhs, skip_list<T,Compare,Allocator> &rhs);

template <class T, class Compare, class Allocator>
bool operator<(skip_list<T,Compare,Allocator> &lhs, skip_list<T,Compare,Allocator> &rhs);

template <class T, class Compare, class Allocator>
bool operator<=(skip_list<T,Compare,Allocator> &lhs, skip_list<T,Compare,Allocator> &rhs);

template <class T, class Compare, class Allocator>
bool operator>(skip_list<T,Compare,Allocator> &lhs, skip_list<T,Compare,Allocator> &rhs);

template <class T, class Compare, class Allocator>
bool operator>=(skip_list<T,Compare,Allocator> &lhs, skip_list<T,Compare,Allocator> &rhs);

namespace std
{
    template <class T, class Compare, class Allocator>
    void swap(skip_list<T,Compare,Allocator> &lhs, skip_list<T,Compare,Allocator> &rhs);
}

//==============================================================================
// iterators

template <class T, class Compare, class Allocator>
class skip_list<T,Compare,Allocator>::iterator
{
public:
};

template <class T, class Compare, class Allocator>
class skip_list<T,Compare,Allocator>::const_iterator
{
public:
    const_iterator() {}
    const_iterator(const iterator &) {}
};

//==============================================================================
// lifetime management

template <class T, class Compare, class Allocator>
inline
skip_list<T,Compare,Allocator>::skip_list(const Allocator &alloc_)
:   alloc(alloc_),
    levels()
{
    nodes[0] = 0;
}

template <class T, class Compare, class Allocator>
inline
skip_list<T,Compare,Allocator>::~skip_list()
{
    clear();
}

template <class T, class Compare, class Allocator>
template <class InputIterator>
inline
skip_list<T,Compare,Allocator>::skip_list(InputIterator first, InputIterator last, const Allocator &alloc_)
:   alloc(alloc_),
    levels(0)
{
    not_implemented_yet();
}

template <class T, class Compare, class Allocator>
inline
skip_list<T,Compare,Allocator>::skip_list(const skip_list &other)
:   alloc(other.alloc),
    levels(0)
{    
    not_implemented_yet();
}

template <class T, class Compare, class Allocator>
inline
skip_list<T,Compare,Allocator>::skip_list(const skip_list &other, const Allocator &alloc_)
:   alloc(alloc_),
    levels(0)
{
    not_implemented_yet();
}

// C++11
//skip_list(const skip_list &&other);
//skip_list(const skip_list &&other, const Allocator &alloc);
//skip_list(std::initializer_list<T> init, const Allocator &alloc = Allocator());

template <class T, class Compare, class Allocator>
inline
Allocator skip_list<T,Compare,Allocator>::get_allocator() const
{
    return alloc;
}

//==============================================================================
// assignment

template <class T, class Compare, class Allocator>
inline
skip_list<T,Compare,Allocator> &skip_list<T,Compare,Allocator>::operator=(const skip_list<T,Compare,Allocator> &other)
{
    not_implemented_yet();
}

//C++11 skip_list& operator=(skip_list&& other);

template <class T, class Compare, class Allocator>
inline
void skip_list<T,Compare,Allocator>::assign(size_type count, const value_type &value)
{
    not_implemented_yet();
}

template <class T, class Compare, class Allocator>
template <typename InputIterator>
inline
void skip_list<T,Compare,Allocator>::assign(InputIterator first, InputIterator last)
{
    not_implemented_yet();
}

//==============================================================================
// element access

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::reference
skip_list<T,Compare,Allocator>::front()
{
    assert_that(nodes[0]);
    return nodes[0]->value;
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_reference
skip_list<T,Compare,Allocator>::front() const
{
    assert_that(nodes[0]);
    return nodes[0]->value;
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::reference
skip_list<T,Compare,Allocator>::back()
{
    not_implemented_yet();
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_reference
skip_list<T,Compare,Allocator>::back() const
{
    not_implemented_yet();
}

//==============================================================================
// iterators

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::iterator
skip_list<T,Compare,Allocator>::begin()
{
    return iterator();
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_iterator
skip_list<T,Compare,Allocator>::begin() const
{
    return const_iterator();
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_iterator
skip_list<T,Compare,Allocator>::cbegin() const
{
    return const_iterator();
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::iterator
skip_list<T,Compare,Allocator>::end()
{
    return iterator();
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_iterator
skip_list<T,Compare,Allocator>::end() const
{
    return const_iterator();
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_iterator
skip_list<T,Compare,Allocator>::cend() const
{
    return const_iterator();
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::reverse_iterator
skip_list<T,Compare,Allocator>::rbegin()
{
    return const_iterator();
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_reverse_iterator
skip_list<T,Compare,Allocator>::rbegin() const
{
    not_implemented_yet();
    return rend();
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_reverse_iterator
skip_list<T,Compare,Allocator>::crbegin() const
{
    not_implemented_yet();
    return rend();
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::reverse_iterator
skip_list<T,Compare,Allocator>::rend()
{
    not_implemented_yet();
    return rend();
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_reverse_iterator
skip_list<T,Compare,Allocator>::rend() const
{
    not_implemented_yet();
    return rend();
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_reverse_iterator
skip_list<T,Compare,Allocator>::crend() const
{
    not_implemented_yet();
    return rend();
}

//==============================================================================
// capacity

template <class T, class Compare, class Allocator>
inline
bool skip_list<T,Compare,Allocator>::empty() const
{
    return nodes[0] == 0;
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::size_type
skip_list<T,Compare,Allocator>::size() const
{
    not_implemented_yet();
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::size_type
skip_list<T,Compare,Allocator>::max_size() const
{
    not_implemented_yet();
}

//==============================================================================
// modifiers

template <class T, class Compare, class Allocator>
inline
void skip_list<T,Compare,Allocator>::clear()
{
    if (nodes[0])
    {
        //node_allocator(alloc).destroy(*nodes[0]);
        //node_allocator(alloc).deallocate(nodes[0]);
    }
    //not_implemented_yet();
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::iterator
skip_list<T,Compare,Allocator>::insert(const_iterator pos, const value_type &value)
{
    if (nodes[0] == 0)
    {
        //assert_that(pos == begin());
        //assert_that(begin() == end());
        nodes[0] = node_allocator(alloc).allocate(1, (void*)0);
        // TODO: construct in-place
        nodes[0]->value = value;
        nodes[0]->next = 0;
        return begin();
    }
    else
    {
        not_implemented_yet();
        return end();
    }
}

//C++11iterator insert const_iterator pos, value_type &&value);

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::iterator
skip_list<T,Compare,Allocator>::insert(const_iterator pos, size_type count, const value_type &value)
{
    not_implemented_yet();
}

template <class T, class Compare, class Allocator>
template <class InputIterator>
inline
typename skip_list<T,Compare,Allocator>::iterator
skip_list<T,Compare,Allocator>::insert(const_iterator pos, InputIterator first, InputIterator last)
{
    not_implemented_yet();
}

//C++11iterator insert(const_iterator pos, std::initializer_list<value_type> ilist);
// C++11 emplace

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::iterator
skip_list<T,Compare,Allocator>::erase(const_iterator position)
{
    not_implemented_yet();
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::iterator
skip_list<T,Compare,Allocator>::erase(const_iterator first, const_iterator last)
{
    not_implemented_yet();
}

template <class T, class Compare, class Allocator>
inline
void skip_list<T,Compare,Allocator>::push_back(const value_type &value)
{
    not_implemented_yet();
}

//C++11void push_back( value_type &&value );
//C++11 emplace_back

template <class T, class Compare, class Allocator>
inline
void skip_list<T,Compare,Allocator>::swap(skip_list<T,Compare,Allocator> &other)
{
    not_implemented_yet();
}

//==============================================================================
// non-members

template <class T, class Compare, class Allocator>
inline
bool operator==(skip_list<T,Compare,Allocator> &lhs, skip_list<T,Compare,Allocator> &rhs)
{
    not_implemented_yet();
    return false;
}

template <class T, class Compare, class Allocator>
inline
bool operator!=(skip_list<T,Compare,Allocator> &lhs, skip_list<T,Compare,Allocator> &rhs)
{
    return !operator=(lhs, rhs);
}

template <class T, class Compare, class Allocator>
inline
bool operator<(skip_list<T,Compare,Allocator> &lhs, skip_list<T,Compare,Allocator> &rhs)
{
    not_implemented_yet();
    return false;
}

template <class T, class Compare, class Allocator>
inline
bool operator<=(skip_list<T,Compare,Allocator> &lhs, skip_list<T,Compare,Allocator> &rhs)
{
    not_implemented_yet();
    return false;
}

template <class T, class Compare, class Allocator>
inline
bool operator>(skip_list<T,Compare,Allocator> &lhs, skip_list<T,Compare,Allocator> &rhs)
{
    not_implemented_yet();
    return false;
}

template <class T, class Compare, class Allocator>
inline
bool operator>=(skip_list<T,Compare,Allocator> &lhs, skip_list<T,Compare,Allocator> &rhs)
{
    not_implemented_yet();
    return false;
}

template <class T, class Compare, class Allocator>
inline
void std::swap(skip_list<T,Compare,Allocator> &lhs, skip_list<T,Compare,Allocator> &rhs)
{
    lhs.swap(rhs);
}

//==============================================================================
// internal

template <class T, class Compare, class Allocator>
inline
unsigned skip_list<T,Compare,Allocator>::random_level()
{
    // TODO: decide on a better approach
    return unsigned(rand()) % max_levels;
}

}
