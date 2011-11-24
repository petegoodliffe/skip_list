//============================================================================
// skip_list.h
// Copyright (c) 2011 Pete Goodliffe. All rights reserved
//============================================================================

#pragma once

#include <memory>

#include <cassert>
void not_implemented_yet() { assert("Not implemented yet" && false); }

/// STL-style skip list container
///
/// C++11: noexect decls
template <typename T, typename Allocator = std::allocator<T> >
class skip_list
{
public:

    //======================================================================
    // types

    typedef T                 value_type;
    typedef Allocator         allocator_type;
    typedef size_t            size_type;
    typedef ptrdiff_t         difference_type;

    // pre C++11
    typedef value_type       &reference;
    typedef const value_type &const_reference;
    typedef T                *pointer;
    typedef const T          *const_pointer;

    // C++11
    //typedef Allocator::reference                            reference;
    //typedef Allocator::const_reference                      const_reference;
    //typedef std::allocator_traits<Allocator>::pointer       pointer;
    //typedef std::allocator_traits<Allocator>::const_pointer const_pointer;

    class iterator;
    class const_iterator;
    class reverse_iterator;
    class const_reverse_iterator;

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

    skip_list &operator=(const skip_list& other);
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
    // operations

    // std:;list has:
    // merge
    // splace
    // remove
    // remove_if
    // reverse
    // unique
    // sort

private:
    struct node
    {
    };

    allocator_type alloc;
};

template <class T, class Allocator>
void operator==(skip_list<T,Allocator> &lhs, skip_list<T,Allocator> &rhs);

template <class T, class Allocator>
void operator!=(skip_list<T,Allocator> &lhs, skip_list<T,Allocator> &rhs);

template <class T, class Allocator>
void operator<(skip_list<T,Allocator> &lhs, skip_list<T,Allocator> &rhs);

template <class T, class Allocator>
void operator<=(skip_list<T,Allocator> &lhs, skip_list<T,Allocator> &rhs);

template <class T, class Allocator>
void operator>(skip_list<T,Allocator> &lhs, skip_list<T,Allocator> &rhs);

template <class T, class Allocator>
void operator>=(skip_list<T,Allocator> &lhs, skip_list<T,Allocator> &rhs);

namespace std
{
    template <class T, class Allocator>
    void swap(skip_list<T,Allocator> &lhs, skip_list<T,Allocator> &rhs);
}

//============================================================================
// lifetime management

template <class T, class Allocator>
inline
skip_list<T,Allocator>::skip_list(const Allocator &alloc)
: alloc(alloc)
{
}

template <class T, class Allocator>
inline
skip_list<T,Allocator>::~skip_list()
{
}

template <class T, class Allocator>
template <class InputIterator>
inline
skip_list<T,Allocator>::skip_list(InputIterator first, InputIterator last, const Allocator &alloc)
: alloc(alloc)
{
    assert(false);
}

template <class T, class Allocator>
inline
skip_list<T,Allocator>::skip_list(const skip_list &other)
: alloc(other.alloc)
{
    assert(false);
}

template <class T, class Allocator>
inline
skip_list<T,Allocator>::skip_list(const skip_list &other, const Allocator &alloc)
: alloc(alloc)
{
    assert(false);
}

// C++11
//skip_list(const skip_list &&other);
//skip_list(const skip_list &&other, const Allocator &alloc);
//skip_list(std::initializer_list<T> init, const Allocator &alloc = Allocator());

template <class T, class Allocator>
inline
Allocator skip_list<T,Allocator>::get_allocator() const
{
    return alloc;
}

//======================================================================
// assignment

template <class T, class Allocator>
inline
skip_list<T,Allocator> &skip_list<T,Allocator>::operator=(const skip_list<T,Allocator> &other)
{
    not_implemented_yet();
}

//C++11 skip_list& operator=(skip_list&& other);

template <class T, class Allocator>
inline
void skip_list<T,Allocator>::assign(size_type count, const value_type &value)
{
    not_implemented_yet();
}

template <class T, class Allocator>
template <typename InputIterator>
inline
void skip_list<T,Allocator>::assign(InputIterator first, InputIterator last)
{
    not_implemented_yet();
}

//======================================================================
// element access

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::reference
skip_list<T,Allocator>::front()
{
    not_implemented_yet();
}

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::const_reference
skip_list<T,Allocator>::front() const
{
    not_implemented_yet();
}

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::reference
skip_list<T,Allocator>::back()
{
    not_implemented_yet();
}

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::const_reference
skip_list<T,Allocator>::back() const
{
    not_implemented_yet();
}

//======================================================================
// iterators

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::iterator
skip_list<T,Allocator>::begin()
{
    not_implemented_yet();
}

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::const_iterator
skip_list<T,Allocator>::begin() const
{
    not_implemented_yet();
}

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::const_iterator
skip_list<T,Allocator>::cbegin() const
{
    not_implemented_yet();
}

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::iterator
skip_list<T,Allocator>::end()
{
    not_implemented_yet();
}

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::const_iterator
skip_list<T,Allocator>::end() const
{
    not_implemented_yet();
}

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::const_iterator
skip_list<T,Allocator>::cend() const
{
    not_implemented_yet();
}

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::reverse_iterator
skip_list<T,Allocator>::rbegin()
{
    not_implemented_yet();
}

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::const_reverse_iterator
skip_list<T,Allocator>::rbegin() const
{
    not_implemented_yet();
}

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::const_reverse_iterator
skip_list<T,Allocator>::crbegin() const
{
    not_implemented_yet();
}

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::reverse_iterator
skip_list<T,Allocator>::rend()
{
    not_implemented_yet();
}

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::const_reverse_iterator
skip_list<T,Allocator>::rend() const
{
    not_implemented_yet();
}

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::const_reverse_iterator
skip_list<T,Allocator>::crend() const
{
    not_implemented_yet();
}

//======================================================================
// capacity

template <class T, class Allocator>
inline
bool skip_list<T,Allocator>::empty() const
{
    not_implemented_yet();
}

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::size_type
skip_list<T,Allocator>::size() const
{
    not_implemented_yet();
}

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::size_type
skip_list<T,Allocator>::max_size() const
{
    not_implemented_yet();
}

//======================================================================
// modifiers

template <class T, class Allocator>
inline
void skip_list<T,Allocator>::clear()
{
    not_implemented_yet();
}

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::iterator
skip_list<T,Allocator>::insert(const_iterator pos, const value_type &value)
{
    not_implemented_yet();
}

//C++11iterator insert const_iterator pos, value_type &&value);

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::iterator
skip_list<T,Allocator>::insert(const_iterator pos, size_type count, const value_type &value)
{
    not_implemented_yet();
}

template <class T, class Allocator>
template <class InputIterator>
inline
typename skip_list<T,Allocator>::iterator
skip_list<T,Allocator>::insert(const_iterator pos, InputIterator first, InputIterator last)
{
    not_implemented_yet();
}

//C++11iterator insert(const_iterator pos, std::initializer_list<value_type> ilist);
// C++11 emplace

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::iterator
skip_list<T,Allocator>::erase(const_iterator position)
{
    not_implemented_yet();
}

template <class T, class Allocator>
inline
typename skip_list<T,Allocator>::iterator
skip_list<T,Allocator>::erase(const_iterator first, const_iterator last)
{
    not_implemented_yet();
}

template <class T, class Allocator>
inline
void skip_list<T,Allocator>::push_back(const value_type &value)
{
    not_implemented_yet();
}

//C++11void push_back( value_type &&value );
//C++11 emplace_back

template <class T, class Allocator>
inline
void skip_list<T,Allocator>::swap(skip_list<T,Allocator> &other)
{
    not_implemented_yet();
}

