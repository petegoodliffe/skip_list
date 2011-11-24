// skip_list.h
// Pete Goodliffe

#pragma once

#include <memory>

/// TODO: allocator
/// C++11: noexpect
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

    skip_list();

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

    skip_list& operator=(const skip_list& other);
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

    void swap(skip_list& other);

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
