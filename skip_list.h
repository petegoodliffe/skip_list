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

    typedef T                                           value_type;
    typedef Allocator                                   allocator_type;
    typedef typename allocator_type::size_type          size_type;
    typedef typename allocator_type::difference_type    difference_type;
    typedef typename allocator_type::reference          reference;
    typedef typename allocator_type::const_reference    const_reference;
    typedef typename allocator_type::pointer            pointer;
    typedef typename allocator_type::const_pointer      const_pointer;
    typedef Compare                                     compare;

    class iterator;
    class const_iterator;
    //typedef std::reverse_iterator<iterator>       reverse_iterator;
    //typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

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

    //reverse_iterator       rbegin();
    //const_reverse_iterator rbegin() const;
    //const_reverse_iterator crbegin() const;

    //reverse_iterator       rend();
    //const_reverse_iterator rend() const;
    //const_reverse_iterator crend() const;

    //======================================================================
    // capacity

    bool      empty() const;
    size_type size() const;
    size_type max_size() const;

    //======================================================================
    // modifiers

    void clear();

    iterator insert(const value_type &value);    
    iterator insert(const_iterator hint, const value_type &value);

    //C++11iterator insert(value_type &&value);
    //C++11iterator insert(const_iterator hint, const value_type &&value);

    template <class InputIterator>
    iterator insert(InputIterator first, InputIterator last);

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
    // lookup

    size_type count(const value_type &value) const;

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
    
public:
    enum { max_levels = 33 };
    static unsigned random_level();
    unsigned new_level();
    
    void dump();

private:
    struct node
    {
        node *next[max_levels];
        value_type value;
    };

    typedef typename allocator_type::template rebind<node>::other node_allocator;
    friend class iterator;
    friend class const_iterator;


    allocator_type  alloc;
    unsigned        levels;
    node            head; // needn't have value
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
    
} // namespace goodliffe

namespace std
{
    template <class T, class Compare, class Allocator>
    void swap(goodliffe::skip_list<T,Compare,Allocator> &lhs, goodliffe::skip_list<T,Compare,Allocator> &rhs);
}

//==============================================================================
// iterators

namespace goodliffe {

template <class T, class Compare, class Allocator>
class skip_list<T,Compare,Allocator>::iterator
    : public std::iterator<std::forward_iterator_tag,
                           typename skip_list<T,Compare,Allocator>::value_type,
                           typename skip_list<T,Compare,Allocator>::difference_type,
                           typename skip_list<T,Compare,Allocator>::pointer,
                           typename skip_list<T,Compare,Allocator>::reference>
{
public:
    typedef skip_list<T,Compare,Allocator>          parent_type;
    typedef typename parent_type::const_iterator    const_type;
    typedef typename parent_type::node              node_type;
    typedef iterator                                self_type;
    
    iterator()
        : parent(0), node(0) {}
    iterator(parent_type &parent_, typename parent_type::node *node_)
        : parent(&parent_), node(node_) {}

    self_type &operator++()
        { node = node->next[0]; return *this; }
    self_type operator++(int) // postincrement
        { self_type old(*this); operator++(); return old; }

    //self_type &operator--()
    //    { --index; return *this; }
    //self_type operator--(int) // postdecrement
    //    { self_type old(*this); operator--(); return old; }
    
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

private:
    friend class const_iterator;
    parent_type *parent;
    node_type   *node;
};

template <class T, class Compare, class Allocator>
class skip_list<T,Compare,Allocator>::const_iterator
    : public std::iterator<std::forward_iterator_tag,
                           typename skip_list<T,Compare,Allocator>::value_type,
                           typename skip_list<T,Compare,Allocator>::difference_type,
                           typename skip_list<T,Compare,Allocator>::const_pointer,
                           typename skip_list<T,Compare,Allocator>::const_reference>
{
public:
    typedef const skip_list<T,Compare,Allocator>    parent_type;
    typedef typename parent_type::iterator          non_const_type;
    typedef const typename parent_type::node        node_type;
    typedef const_iterator                          self_type;

    const_iterator()
        : parent(0), node(0) {}
    const_iterator(const non_const_type &i)
        : parent(i.parent), node(i.node) {}
    const_iterator(const parent_type &parent_, node_type *node_)
        : parent(&parent_), node(node_) {}

    self_type &operator++()
        { node = node->next; return *this; }
    self_type operator++(int) // postincrement
        { self_type old(*this); operator++(); return old; }

    //self_type &operator--()
    //    { --index; return *this; }
    //self_type operator--(int) // postdecrement
    //    { self_type old(*this); operator--(); return old; }
    
    //size_type operator-(const self_type &other) const
    //    { return index - other.index; }
    
    const_reference operator*()  { return &node->value; }
    const_pointer   operator->() { return node->value; }
    
    bool operator==(const self_type &other) const
        { return parent == other.parent && node == other.node; }
    bool operator!=(const self_type &other) const
        { return !operator==(other); }

    bool operator==(const non_const_type &other) const
        { return parent == other.parent && node == other.node; }
    bool operator!=(const non_const_type &other) const
        { return !operator==(other); }

private:
    friend class iterator;
    parent_type *parent;
    node_type   *node;
};

//==============================================================================
// lifetime management

template <class T, class Compare, class Allocator>
inline
skip_list<T,Compare,Allocator>::skip_list(const Allocator &alloc_)
:   alloc(alloc_),
    levels()
{
    for (unsigned n = 0; n < max_levels; n++)
        head.next[n] = 0;
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
    assert_that(head.next[0]);
    return head.next[0]->value;
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_reference
skip_list<T,Compare,Allocator>::front() const
{
    assert_that(head.next[0]);
    return head.next[0]->value;
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
    return iterator(*this, head.next[0]);
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_iterator
skip_list<T,Compare,Allocator>::begin() const
{
    return const_iterator(*this, head.next[0]);
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_iterator
skip_list<T,Compare,Allocator>::cbegin() const
{
    return const_iterator(*this, head.next[0]);
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::iterator
skip_list<T,Compare,Allocator>::end()
{
    return iterator(*this, 0);
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_iterator
skip_list<T,Compare,Allocator>::end() const
{
    return const_iterator(*this, 0);
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::const_iterator
skip_list<T,Compare,Allocator>::cend() const
{
    return const_iterator(*this, 0);
}
/*
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
*/
//==============================================================================
// capacity

template <class T, class Compare, class Allocator>
inline
bool skip_list<T,Compare,Allocator>::empty() const
{
    return levels == 0;
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
    return alloc.max_size();
}

//==============================================================================
// modifiers

template <class T, class Compare, class Allocator>
inline
void skip_list<T,Compare,Allocator>::clear()
{
    //if (nodes[0])
    {
        //node_allocator(alloc).destroy(*nodes[0]);
        //node_allocator(alloc).deallocate(nodes[0]);
    }
    //not_implemented_yet();
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::iterator
skip_list<T,Compare,Allocator>::insert(const value_type &value)
{
    const unsigned level = new_level();

    node *new_node = node_allocator(alloc).allocate(1, (void*)0);
    // TODO: construct in-place
    new_node->value = value;
    for (unsigned n = 0; n < max_levels; ++n)
        new_node->next[n] = 0;

    node *insert_point = &head;
    for (unsigned l = levels; l; )
    {
        --l;
        while (insert_point->next[l] && insert_point->next[l]->value < value)
            insert_point = insert_point->next[l];

        if (l <= level)
        {
            new_node->next[l] = insert_point->next[l];
            insert_point->next[l] = new_node;
        }
    }

    return iterator(*this, new_node);
}

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::iterator
skip_list<T,Compare,Allocator>::insert(const_iterator hint, const value_type &value)
{
    // TODO - try to optimse lookup using this
    (void) hint;
    return insert(value);
}

//C++11iterator insert const_iterator pos, value_type &&value);

template <class T, class Compare, class Allocator>
template <class InputIterator>
inline
typename skip_list<T,Compare,Allocator>::iterator
skip_list<T,Compare,Allocator>::insert(InputIterator first, InputIterator last)
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
// lookup

template <class T, class Compare, class Allocator>
inline
typename skip_list<T,Compare,Allocator>::size_type
skip_list<T,Compare,Allocator>::count(const value_type &value) const
{
    // TODO: == <=
    const node *search = &head;
    for (int i = int(levels-1); i >= 0; i--)
    {
        while (search->next[i] && search->next[i]->value <= value)
            search = search->next[i];
    }
    return search != &head && search->value == value;
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
    
} // namespace goodliffe

template <class T, class Compare, class Allocator>
inline
void std::swap(goodliffe::skip_list<T,Compare,Allocator> &lhs, goodliffe::skip_list<T,Compare,Allocator> &rhs)
{
    lhs.swap(rhs);
}

namespace goodliffe {

//==============================================================================
// internal

/// Generate a stream of levels, probabilstically chosen.
/// With a probability of 1/2, return 0.
/// With 1/4 probability, return 1.
/// With 1/8 probability, return 2.
/// And so forth.
/// TODO: Consider making this a template method
template <class T, class Compare, class Allocator>
inline
unsigned skip_list<T,Compare,Allocator>::random_level()
{
    // The number of 1-bits before we encounter the first 0-bit is the level of
    /// the node. Since R is 32-bit, the level can be at most 32.
    unsigned level = 0;
    for (unsigned number = unsigned(rand()); (number & 1) == 1; number >>= 1)
    {
        level++;
        //if (level == levels) { levels++; break; }
    }
    return level;
}

template <class T, class Compare, class Allocator>
inline
unsigned skip_list<T,Compare,Allocator>::new_level()
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
void skip_list<T,Compare,Allocator>::dump()
{
    printf("skip_list(levels=%u)\n", levels);
    for (unsigned l = 0; l < levels; ++l)
    {
        printf("  %u: ", l);
        node *n = head.next[l];
        while (n)
        {
            printf("> %d ", n->value);
            n = n->next[l];
        }
        printf("\n");
    }
}

}
