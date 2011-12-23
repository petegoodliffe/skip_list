//==============================================================================
// random_access_skip_list.h
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
namespace detail
{
    template <typename T,typename C,typename A,typename LG>
    class rasl_impl;

    template <typename LIST> class random_access_skip_list_iterator;
    template <typename LIST> class random_access_skip_list_const_iterator;
}
}

//==============================================================================
#pragma mark - random_access_skip_list
//==============================================================================

namespace goodliffe {

/// A random_access_skip_list is a skip_list variant that provides
/// O(log N) random access and random access iterators.
///
/// That is, it provides relatively fast operator[] and the ability to
/// write:
///     iterator i = list.begin()
///     i += 5;
/// 
/// This speed comes at the expense of a little extra storage within the
/// data structure.
///
/// @see skip_list
template <typename T,
          typename Compare        = std::less<T>,
          typename Allocator      = std::allocator<T>,
          typename LevelGenerator = detail::skip_list_level_generator<32> >
class random_access_skip_list
{
private:
    typedef typename detail::rasl_impl<T,Compare,Allocator,LevelGenerator> impl_type;
    typedef typename impl_type::node_type                                  node_type;

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
    
    typedef typename detail::random_access_skip_list_iterator<impl_type> iterator;
    typedef typename iterator::const_iterator                            const_iterator;
    typedef std::reverse_iterator<iterator>                              reverse_iterator;
    typedef std::reverse_iterator<const_iterator>                        const_reverse_iterator;

    //======================================================================
    // lifetime management

    explicit random_access_skip_list(const Allocator &alloc = Allocator());

    template <class InputIterator>
    random_access_skip_list(InputIterator first, InputIterator last, const Allocator &alloc = Allocator());

    random_access_skip_list(const random_access_skip_list &other);
    random_access_skip_list(const random_access_skip_list &other, const Allocator &alloc);

    // C++11
    //random_access_skip_list(const random_access_skip_list &&other);
    //random_access_skip_list(const random_access_skip_list &&other, const Allocator &alloc);
    //random_access_skip_list(std::initializer_list<T> init, const Allocator &alloc = Allocator());

    allocator_type get_allocator() const { return impl.get_allocator(); }

    //======================================================================
    // assignment

    random_access_skip_list &operator=(const random_access_skip_list &other);
    //C++11 random_access_skip_list& operator=(random_access_skip_list&& other);

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

    void swap(random_access_skip_list &other) { impl.swap(other.impl); }

    friend void swap(random_access_skip_list &lhs, random_access_skip_list &rhs) { lhs.swap(rhs); }

    //======================================================================
    // lookup

    bool           contains(const value_type &value) const { return count(value) != 0; }
    size_type      count(const value_type &value) const;

    iterator       find(const value_type &value);
    const_iterator find(const value_type &value) const;

    //======================================================================
    // random access

    const_reference operator[](unsigned index) const;
    
    iterator       iterator_at(unsigned index);
    const_iterator iterator_at(unsigned index) const;
    const_iterator citerator_at(unsigned index) const { return iterator_at(index); }
    
    size_type index_of(const const_iterator &i) const;

    void erase_at(size_type index);

    //======================================================================
    // other operations

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
bool operator==(const random_access_skip_list<T,C,A,LG> &lhs, const random_access_skip_list<T,C,A,LG> &rhs)
{
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T, class C, class A, class LG>
inline
bool operator!=(const random_access_skip_list<T,C,A,LG> &lhs, const random_access_skip_list<T,C,A,LG> &rhs)
{
    return !operator==(lhs, rhs);
}

template <class T, class C, class A, class LG>
inline
bool operator<(const random_access_skip_list<T,C,A,LG> &lhs, const random_access_skip_list<T,C,A,LG> &rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <class T, class C, class A, class LG>
inline
bool operator<=(const random_access_skip_list<T,C,A,LG> &lhs, const random_access_skip_list<T,C,A,LG> &rhs)
{
    return !(rhs < lhs);
}

template <class T, class C, class A, class LG>
inline
bool operator>(const random_access_skip_list<T,C,A,LG> &lhs, const random_access_skip_list<T,C,A,LG> &rhs)
{
    return rhs < lhs;
}

template <class T, class C, class A, class LG>
inline
bool operator>=(const random_access_skip_list<T,C,A,LG> &lhs, const random_access_skip_list<T,C,A,LG> &rhs)
{
    return !(lhs < rhs);
}

} // namespace goodliffe

namespace std
{
    template <class T, class C, class A, class LG>
    void swap(goodliffe::random_access_skip_list<T,C,A,LG> &lhs, goodliffe::random_access_skip_list<T,C,A,LG> &rhs)
    {
        lhs.swap(rhs);
    }
}

//==============================================================================
#pragma mark - iterators
//==============================================================================

namespace goodliffe {
namespace detail {

template <typename rasl_impl>
class random_access_skip_list_iterator
    : public std::iterator<std::random_access_iterator_tag,
                           typename rasl_impl::value_type,
                           typename rasl_impl::difference_type,
                           typename rasl_impl::const_pointer,
                           typename rasl_impl::const_reference>
{
public:
    typedef rasl_impl                                          impl_type;
    typedef random_access_skip_list_const_iterator<rasl_impl>  const_iterator;
    typedef typename impl_type::node_type                      node_type;
    typedef random_access_skip_list_iterator<rasl_impl>        self_type;

    typedef typename impl_type::difference_type                difference_type;

    random_access_skip_list_iterator()
        : impl(0), node(0) {}
    random_access_skip_list_iterator(impl_type *impl_, node_type *node_)
        : impl(impl_), node(node_) {}
    random_access_skip_list_iterator(const random_access_skip_list_iterator &other)
        : impl(other.impl), node(other.node) {}

    self_type &operator++()
        { node = node->next[0]; return *this; }
    self_type operator++(int) // postincrement
        { self_type old(*this); node = node->next[0]; return old; }

    self_type &operator--()
        { node = node->prev; return *this; }
    self_type operator--(int) // postdecrement
        { self_type old(*this); node = node->prev; return old; }
    
    self_type &operator+=(difference_type n)
        { node = impl->at(impl->index_of(node)+n); return *this; }
    self_type &operator-=(difference_type n)
        { node = impl->at(impl->index_of(node)-n); return *this; }

    random_access_skip_list_iterator operator+(difference_type rhs) const
        { return random_access_skip_list_iterator(*this) += rhs; }
    random_access_skip_list_iterator operator-(difference_type rhs) const
        { return random_access_skip_list_iterator(*this) -= rhs; }
    typename impl_type::const_reference operator[](int index) const; // TODO
    bool operator<(const self_type &other) const; // TODO

    typename impl_type::const_reference operator*()  { return node->value; }
    typename impl_type::const_pointer   operator->() { return node->value; }
    
    bool operator==(const self_type &other) const
        { return impl == other.impl && node == other.node; }
    bool operator!=(const self_type &other) const
        { return !operator==(other); }
    
    bool operator==(const const_iterator &other) const
        { return impl == other.get_impl() && node == other.node; }
    bool operator!=(const const_iterator &other) const
        { return !operator==(other); }

    const impl_type *get_impl() const { return impl; } ///< @internal
    const node_type *get_node() const { return node; }   ///< @internal

private:
    impl_type *impl;
    node_type *node;
};

template <typename I>
inline
random_access_skip_list_iterator<I> operator+(int lhs, random_access_skip_list_iterator<I> rhs)
    { return rhs+lhs; }
template <typename I>
inline
random_access_skip_list_iterator<I> operator-(int lhs, random_access_skip_list_iterator<I> rhs)
    { return rhs-lhs; } // TODO: checl


template <typename rasl_impl>
class random_access_skip_list_const_iterator
    : public std::iterator<std::random_access_iterator_tag,
                           typename rasl_impl::value_type,
                           typename rasl_impl::difference_type,
                           typename rasl_impl::const_pointer,
                           typename rasl_impl::const_reference>
{
public:
    typedef const rasl_impl                                    impl_type;
    typedef random_access_skip_list_iterator<rasl_impl>        normal_iterator;
    typedef const typename impl_type::node_type                node_type;
    typedef random_access_skip_list_const_iterator<rasl_impl>  self_type;

    random_access_skip_list_const_iterator()
        : impl(0), node(0) {}
    random_access_skip_list_const_iterator(const normal_iterator &i)
        : impl(i.get_impl()), node(i.get_node()) {}
    random_access_skip_list_const_iterator(const impl_type *impl_, node_type *node_)
        : impl(impl_), node(node_) {}

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
        { return impl == other.impl && node == other.node; }
    bool operator!=(const self_type &other) const
        { return !operator==(other); }

    bool operator==(const normal_iterator &other) const
        { return impl == other.impl && node == other.node; }
    bool operator!=(const normal_iterator &other) const
        { return !operator==(other); }

    const impl_type *get_impl() const { return impl; } ///< @internal
    const node_type *get_node() const { return node; } ///< @internal

private:
    impl_type *impl;
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
random_access_skip_list<T,C,A,LG>::random_access_skip_list(const allocator_type &alloc_)
:   impl(alloc_)
{
}

template <class T, class C, class A, class LG>
template <class InputIterator>
inline
random_access_skip_list<T,C,A,LG>::random_access_skip_list(InputIterator first, InputIterator last, const allocator_type &alloc_)
:   impl(alloc_)
{
    assign(first, last);
}

template <class T, class C, class A, class LG>
inline
random_access_skip_list<T,C,A,LG>::random_access_skip_list(const random_access_skip_list &other)
:   impl(other.get_allocator())
{    
    assign(other.begin(), other.end());
}

template <class T, class C, class A, class LG>
inline
random_access_skip_list<T,C,A,LG>::random_access_skip_list(const random_access_skip_list &other, const allocator_type &alloc_)
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
random_access_skip_list<T,C,A,LG> &
random_access_skip_list<T,C,A,LG>::operator=(const random_access_skip_list<T,C,A,LG> &other)
{
    assign(other.begin(), other.end());
    return *this;
}

//C++11 skip_list& operator=(skip_list&& other);

template <class T, class C, class A, class LG>
template <typename InputIterator>
inline
void random_access_skip_list<T,C,A,LG>::assign(InputIterator first, InputIterator last)
{
    clear();
    while (first != last) insert(*first++);
}

//==============================================================================
#pragma mark element access

template <class T, class C, class A, class LG>
inline
typename random_access_skip_list<T,C,A,LG>::reference
random_access_skip_list<T,C,A,LG>::front()
{
    assert_that(!empty());
    return impl.front()->value;
}

template <class T, class C, class A, class LG>
inline
typename random_access_skip_list<T,C,A,LG>::const_reference
random_access_skip_list<T,C,A,LG>::front() const
{
    assert_that(!empty());
    return impl.front()->value;
}

template <class T, class C, class A, class LG>
inline
typename random_access_skip_list<T,C,A,LG>::reference
random_access_skip_list<T,C,A,LG>::back()
{
    assert_that(!empty());
    return impl.one_past_end()->prev->value;
}

template <class T, class C, class A, class LG>
inline
typename random_access_skip_list<T,C,A,LG>::const_reference
random_access_skip_list<T,C,A,LG>::back() const
{
    assert_that(!empty());
    return impl.one_past_end()->prev->value;
}

//==============================================================================
#pragma mark modifiers

template <class T, class C, class A, class LG>
inline
void random_access_skip_list<T,C,A,LG>::clear()
{
    impl.remove_all();
}

template <class T, class C, class A, class LG>
inline
typename random_access_skip_list<T,C,A,LG>::insert_by_value_result
random_access_skip_list<T,C,A,LG>::insert(const value_type &value)
{
    node_type *node = impl.insert(value);
    return std::make_pair(iterator(&impl, node), impl.is_valid(node));
}

template <class T, class C, class A, class LG>
inline
typename random_access_skip_list<T,C,A,LG>::iterator
random_access_skip_list<T,C,A,LG>::insert(const_iterator hint, const value_type &value)
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
random_access_skip_list<T,C,A,LG>::insert(InputIterator first, InputIterator last)
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
typename random_access_skip_list<T,C,A,LG>::size_type
random_access_skip_list<T,C,A,LG>::erase(const value_type &value)
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
typename random_access_skip_list<T,C,A,LG>::iterator
random_access_skip_list<T,C,A,LG>::erase(const_iterator position)
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
typename random_access_skip_list<T,C,A,LG>::iterator
random_access_skip_list<T,C,A,LG>::erase(const_iterator first, const_iterator last)
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
typename random_access_skip_list<T,C,A,LG>::size_type
random_access_skip_list<T,C,A,LG>::count(const value_type &value) const
{
    const node_type *node = impl.find(value);
    return impl.is_valid(node) && detail::equivalent(node->value, value, impl.less);
}

template <class T, class C, class A, class LG>
inline
typename random_access_skip_list<T,C,A,LG>::iterator
random_access_skip_list<T,C,A,LG>::find(const value_type &value)
{
    node_type *node = impl.find(value);
    return impl.is_valid(node) && detail::equivalent(node->value, value, impl.less)
        ? iterator(&impl, node)
        : end();
}
  
template <class T, class C, class A, class LG>
inline
typename random_access_skip_list<T,C,A,LG>::const_iterator
random_access_skip_list<T,C,A,LG>::find(const value_type &value) const
{
    const node_type *node = impl.find(value);
    return impl.is_valid(node) && detail::equivalent(node->value, value, impl.less)
        ? const_iterator(&impl, node)
        : end();
}
    
//==============================================================================
#pragma mark random access

template <class T, class C, class A, class LG>
inline
typename random_access_skip_list<T,C,A,LG>::const_reference
random_access_skip_list<T,C,A,LG>::operator[](unsigned index) const
{
    const node_type *node = impl.at(index);
    assert_that(impl.is_valid(node));
    return node->value;
}

template <class T, class C, class A, class LG>
inline
void
random_access_skip_list<T,C,A,LG>::erase_at(size_type index)
{
    node_type *node = impl.at(index);
    assert_that(impl.is_valid(node));
    impl.remove(node);
}

template <class T, class C, class A, class LG>
inline
typename random_access_skip_list<T,C,A,LG>::iterator
random_access_skip_list<T,C,A,LG>::iterator_at(unsigned index)
{
    node_type *node = impl.at(index);
    return iterator(&impl, node);
}

template <class T, class C, class A, class LG>
inline
typename random_access_skip_list<T,C,A,LG>::const_iterator
random_access_skip_list<T,C,A,LG>::iterator_at(unsigned index) const
{
    const node_type *node = impl.at(index);
    return const_iterator(&impl, node);
}

template <class T, class C, class A, class LG>
inline
typename random_access_skip_list<T,C,A,LG>::size_type
random_access_skip_list<T,C,A,LG>::index_of(const const_iterator &i) const
{
    return impl.index_of(i.get_node());
}

} // namespace goodliffe

//==============================================================================
#pragma mark - rasl_impl

namespace goodliffe {
namespace detail {

template <typename T, typename SPAN>
struct rasl_node
{
    typedef SPAN size_type;
    
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    unsigned    magic;
#endif
    T           value;
    unsigned    level;
    rasl_node  *prev;
    rasl_node **next; ///< effectively node_type *next[level+1];
    size_type  *span; ///< effectively unsigned span[level+1];
};

/// Internal implementation of skip_list data structure and methods for
/// modifying it.
///
/// Not for "public" access.
///
/// @internal
template <typename T, typename Compare, typename Allocator, typename LevelGenerator>
class rasl_impl
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
    typedef rasl_node<T, size_type>             node_type;

    static const unsigned num_levels = LevelGenerator::num_levels;

    rasl_impl(const Allocator &alloc = Allocator());
    ~rasl_impl();

    Allocator        get_allocator() const                 { return alloc; }
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
    void             swap(rasl_impl &other);
    size_type        index_of(const node_type *node) const;

    template <typename STREAM>
    void        dump(STREAM &stream) const;
    bool        check() const;
    unsigned    new_level();

    compare_type less;

private:
    typedef typename Allocator::template rebind<node_type>::other    node_allocator;
    typedef typename Allocator::template rebind<node_type*>::other   list_allocator;
    typedef typename Allocator::template rebind<size_type>::other    span_allocator;

    rasl_impl(const rasl_impl &other);
    rasl_impl &operator=(const rasl_impl &other);
    
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
        
    node_type *allocate(unsigned level)
    {
        node_type *node = node_allocator(alloc).allocate(1, (void*)0);
        node->next  = list_allocator(alloc).allocate(level+1, (void*)0);
        node->span  = span_allocator(alloc).allocate(level+1, (void*)0);
        node->level = level;
        for (unsigned n = 0; n <= level; ++n) node->span[n] = 1;
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
        for (unsigned n = 0; n <= level; ++n) node->next[n] = 0;
        node->magic = MAGIC_GOOD;
#endif
        return node;
    }

    void deallocate(node_type *node)
    {
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
        assert_that(node->magic == MAGIC_GOOD);
        node->magic = MAGIC_BAD;
        for (unsigned n = 0; n <= node->level; ++n) node->next[n] = 0;
        node->prev = 0;
#endif
        span_allocator(alloc).deallocate(node->span, node->level+1);
        list_allocator(alloc).deallocate(node->next, node->level+1);
        node_allocator(alloc).deallocate(node, 1);
    }
};

template <class T, class C, class A, class LG>
inline
rasl_impl<T,C,A,LG>::rasl_impl(const allocator_type &alloc_)
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
        head->span[n] = 1;
    }
    head->prev = 0;
    tail->prev = head;
}

template <class T, class C, class A, class LG>
inline
rasl_impl<T,C,A,LG>::~rasl_impl()
{
    remove_all();
    deallocate(head);
    deallocate(tail);
}

template <class T, class C, class A, class LG>
inline
typename rasl_impl<T,C,A,LG>::node_type *
rasl_impl<T,C,A,LG>::find(const value_type &value) const
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

template <class T, class C, class A, class LG>
inline
typename rasl_impl<T,C,A,LG>::size_type
rasl_impl<T,C,A,LG>::find_chain(const value_type &value, node_type **chain, size_type *indexes) const
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
            index += cur->span[l];
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
template <class T, class C, class A, class LG>
inline
typename rasl_impl<T,C,A,LG>::size_type
rasl_impl<T,C,A,LG>::find_chain(const node_type *node, node_type **chain, size_type *indexes) const
{
    assert_that(node && node != head);
    if (node == tail) return find_end_chain(chain, indexes);
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
            index += cur->span[l];
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
template <class T, class C, class A, class LG>
inline
typename rasl_impl<T,C,A,LG>::size_type
rasl_impl<T,C,A,LG>::find_end_chain(node_type **chain, size_type *indexes) const
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
            index += cur->span[l];
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
template <class T, class C, class A, class LG>
inline
typename rasl_impl<T,C,A,LG>::node_type*
rasl_impl<T,C,A,LG>::insert(const value_type &value, node_type *hint)
{
    const unsigned level = new_level();

    node_type *new_node = allocate(level);
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
            if (l>0)
                ++chain[l]->span[l];
        }
        else
        {
            new_node->next[l] = chain[l]->next[l];
            chain[l]->next[l] = new_node;
            size_type prev_span = chain[l]->span[l];
            chain[l]->span[l] = index+1-indexes[l];
            new_node->span[l] = prev_span - (index-indexes[l]);
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

template <class T, class C, class A, class LG>
inline
void
rasl_impl<T,C,A,LG>::remove(node_type *node)
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
            chain[l]->span[l] = chain[l]->span[l] + node->span[l]-1;
            chain[l]->next[l] = node->next[l];
        }
        else if (chain[l] == head || less(chain[l]->value, node->value))
        {
            if (l > 0)
                --chain[l]->span[l];
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
rasl_impl<T,C,A,LG>::remove_all()
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
    {
        head->next[l] = tail;
        head->span[l] = 1;
    }
    tail->prev = head;
    item_count = 0;
        
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    check();
#endif
}

template <class T, class C, class A, class LG>
inline
void 
rasl_impl<T,C,A,LG>::remove_between(node_type *first, node_type *last)
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
            << "  span=" << last_chain[n]->span[n] << "\n";
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
        first_chain[l]->span[l] = last_indexes[l]+last_chain[l]->span[l]-first_indexes[l]-size_reduction;
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
typename rasl_impl<T,C,A,LG>::node_type *
rasl_impl<T,C,A,LG>::at(size_type index)
{
    assert_that(index < item_count);

    unsigned l = levels;
    node_type *node = head;
    index += 1;

    while (l)
    {
        --l;
        while (node->span[l] <= index)
        {
            index -= node->span[l];
            node = node->next[l];
        }
    }

    return node;
}

template <class T, class C, class A, class LG>
inline
const typename rasl_impl<T,C,A,LG>::node_type *
rasl_impl<T,C,A,LG>::at(size_type index) const
{
    return const_cast<rasl_impl*>(this)->at(index);
}

template <class T, class C, class A, class LG>
inline
typename rasl_impl<T,C,A,LG>::size_type
rasl_impl<T,C,A,LG>::index_of(const node_type *node) const
{
    node_type *chain[num_levels]   = {0};
    size_type  indexes[num_levels] = {0};
    return find_chain(node, chain, indexes);
}

template <class T, class C, class A, class LG>
inline
unsigned rasl_impl<T,C,A,LG>::new_level()
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
void rasl_impl<T,C,A,LG>::swap(rasl_impl &other)
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
template <class T, class C, class A, class LG>
template <class STREAM>
inline
void rasl_impl<T,C,A,LG>::dump(STREAM &s) const
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
            size_type span = n->span[l];
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
template <class T, class C, class A, class LG>
inline
bool rasl_impl<T,C,A,LG>::check() const
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
