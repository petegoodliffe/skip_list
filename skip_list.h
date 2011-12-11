//==============================================================================
// skip_list.h
// Copyright (c) 2011 Pete Goodliffe. All rights reserved.
//==============================================================================

#pragma once

#include <memory>     // for std::allocator
#include <functional> // for std::less
#include <iterator>   // for std::reverse_iterator
#include <utility>    // for std::pair
#include <cmath>      // for std::log
#include <cstdlib>    // for std::rand

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
    template <unsigned NumLevels> class bit_based_skip_list_level_generator;
    template <unsigned NumLevels> class skip_list_level_generator;
    template <typename T>         struct skip_list_node;
    template <typename T>         struct skip_list_node_with_span;
    
    template <typename T,typename C,typename A,unsigned NL,typename LG,typename N>
    class skip_list_impl;
}
}

//==============================================================================
#pragma mark - skip_list

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
          unsigned NumLevels      = 32,
          typename LevelGenerator = detail::skip_list_level_generator<NumLevels>,
          typename SkipListType   = typename detail::skip_list_impl<T,Compare,Allocator,NumLevels,LevelGenerator,detail::skip_list_node<T> > >
class skip_list
{
protected:
    typedef SkipListType                    impl_type;
    typedef typename impl_type::node_type   node_type;

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
    friend class iterator;
    friend class const_iterator;

    impl_type impl;
};

template <class T, class C, class A, unsigned NL, class LG, class SLT>
bool operator==(const skip_list<T,C,A,NL,LG,SLT> &lhs, const skip_list<T,C,A,NL,LG,SLT> &rhs);

template <class T, class C, class A, unsigned NL, class LG, class SLT>
bool operator!=(const skip_list<T,C,A,NL,LG,SLT> &lhs, const skip_list<T,C,A,NL,LG,SLT> &rhs);

template <class T, class C, class A, unsigned NL, class LG, class SLT>
bool operator<(const skip_list<T,C,A,NL,LG,SLT> &lhs, const skip_list<T,C,A,NL,LG,SLT> &rhs);

template <class T, class C, class A, unsigned NL, class LG, class SLT>
bool operator<=(const skip_list<T,C,A,NL,LG,SLT> &lhs, const skip_list<T,C,A,NL,LG,SLT> &rhs);

template <class T, class C, class A, unsigned NL, class LG, class SLT>
bool operator>(const skip_list<T,C,A,NL,LG,SLT> &lhs, const skip_list<T,C,A,NL,LG,SLT> &rhs);

template <class T, class C, class A, unsigned NL, class LG, class SLT>
bool operator>=(const skip_list<T,C,A,NL,LG,SLT> &lhs, const skip_list<T,C,A,NL,LG,SLT> &rhs);
    
} // namespace goodliffe

namespace std
{
    template <class T, class C, class A, unsigned NL, class LG, class SLT>
    void swap(goodliffe::skip_list<T,C,A,NL,LG,SLT> &lhs, goodliffe::skip_list<T,C,A,NL,LG,SLT> &rhs);
}

//==============================================================================

namespace goodliffe {

/// A random_access_skip_list is a skip_list variant that provides
/// O(log N) random access.
///
/// That is, it provides relatively fast operator[]
/// 
/// This speed comes at the expense of a little extra storage within the
/// data structure.
template <typename T,
          typename Compare        = std::less<T>,
          typename Allocator      = std::allocator<T>,
          unsigned NumLevels      = 32,
          typename LevelGenerator = detail::skip_list_level_generator<NumLevels> >
class random_access_skip_list :
    public skip_list
        <
            T, Compare, Allocator, NumLevels, LevelGenerator,
            typename detail::skip_list_impl
                <
                    T,Compare,Allocator,NumLevels,LevelGenerator,
                    detail::skip_list_node_with_span<T>
                >
        >
{
private:
    typedef skip_list
        <
            T, Compare, Allocator, NumLevels, LevelGenerator,
            typename detail::skip_list_impl
                <
                    T,Compare,Allocator,NumLevels,LevelGenerator,
                    detail::skip_list_node_with_span<T> 
                >
        >
        parent_type;

protected:
    using typename parent_type::impl_type;
    using typename parent_type::node_type;
    using parent_type::impl;

public:

    //======================================================================
    // types

    using typename parent_type::value_type;
    using typename parent_type::allocator_type;
    using typename parent_type::size_type;
    using typename parent_type::difference_type;
    using typename parent_type::reference;
    using typename parent_type::const_reference;
    using typename parent_type::pointer;
    using typename parent_type::const_pointer;
    using typename parent_type::compare;
    
    using typename parent_type::iterator;
    using typename parent_type::const_iterator;
    using typename parent_type::reverse_iterator;
    using typename parent_type::const_reverse_iterator;
    
    //======================================================================
    // lifetime management
    
    explicit random_access_skip_list(const Allocator &alloc = Allocator())
        : parent_type(alloc) {}
    
    template <class InputIterator>
    random_access_skip_list(InputIterator first, InputIterator last, const Allocator &alloc = Allocator())
        : parent_type(first, last, alloc) {}
    
    random_access_skip_list(const random_access_skip_list &other)
        : parent_type(other) {}
    random_access_skip_list(const random_access_skip_list &other, const Allocator &alloc)
        : parent_type(other, alloc) {}
    
    // C++11
    //random_access_skip_list(const skip_list &&other);
    //random_access_skip_list(const skip_list &&other, const Allocator &alloc);
    //random_access_skip_list(std::initializer_list<T> init, const Allocator &alloc = Allocator());
    
    //======================================================================
    // random access

    const_reference operator[](unsigned index) const;
    
    iterator       iterator_at(unsigned index);
    const_iterator iterator_at(unsigned index) const;

    void erase_at(size_type index);
};

} // namespace goodliffe

//==============================================================================
#pragma mark - diagnostics

//#define SKIP_LIST_IMPL_DIAGNOSTICS 1

#if defined(DEBUG) || defined(_DEBUG) || defined(SKIP_LIST_IMPL_DIAGNOSTICS)
#define SKIP_LIST_DIAGNOSTICS 1
#endif

#ifdef SKIP_LIST_DIAGNOSTICS

void pg_assertion_break();
inline
void pg_assertion_break() { fprintf(stderr, "**** place a breakpoint at pg_assertion_break to debug\n"); }
#include <cassert>
#include <stdio.h>
#define pg_fail(a)            {fprintf(stderr,"%s:%d: \"%s\"\n", __FILE__, __LINE__, a); /*assert(false);*/ pg_assertion_break(); }
#define assert_that(a)        {if (!(a)) pg_fail(#a);}
#define pg_not_implemented_yet() pg_fail("not implemented yet")

#else

#define pg_fail(a)            
#define assert_that(a)        
#define pg_not_implemented_yet() 

#endif

namespace goodliffe {
namespace detail {

template<bool> struct static_assert_that_impl;
template<> struct static_assert_that_impl<true> {};
    
#define static_assert_that(a) \
    {::goodliffe::detail::static_assert_that_impl<a> foo;(void)foo;}

} // namespace detail
} // namespace goodliffe

//==============================================================================
#pragma mark - skip_list_impl declaration

namespace goodliffe {
namespace detail {

/// Generate a stream of levels, probabilstically chosen.
/// - With a probability of 1/2, return 0.
/// - With 1/4 probability, return 1.
/// - With 1/8 probability, return 2.
/// - And so forth.
template <unsigned NumLevels>
class skip_list_level_generator
{
public:
    static const unsigned num_levels = NumLevels;
    unsigned new_level();
};

template <unsigned NumLevels>
class bit_based_skip_list_level_generator
{
public:
    static const unsigned num_levels = NumLevels;
    unsigned new_level();
};

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
    
template <typename T>
struct skip_list_node_with_span
{
    typedef skip_list_node_with_span<T> self_type;
    
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    unsigned    magic;
#endif
    T           value;
    unsigned    level;
    self_type  *prev;
    self_type **next; ///< effectively node_type *next[level+1];
    unsigned   *span; ///< effectively unsigned span[level+1];
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
          unsigned NumLevels, typename LevelGenerator,
          typename NodeType>
class skip_list_impl
{
public:
    
    typedef T                             value_type;
    typedef typename Allocator::size_type size_type;
    typedef Allocator                     allocator_type;
    typedef Compare                       compare_type;
    typedef LevelGenerator                generator_type;
    typedef NodeType                      node_type;

    static const unsigned num_levels = NumLevels;

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
    node_type       *at(size_type index);
    const node_type *at(size_type index) const;
    node_type       *insert(const value_type &value, node_type *hint = 0);
    void             remove(node_type *value);
    void             remove_all();
    void             remove_between(node_type *first, node_type *last);
    void             swap(skip_list_impl &other);

    unsigned         new_level();

    template <typename STREAM>
    void dump(STREAM &stream) const;
    bool check() const;

    compare_type less;

private:
    typedef skip_list_node_traits<node_type, Allocator> node_traits;

    skip_list_impl(const skip_list_impl &other);
    skip_list_impl &operator=(const skip_list_impl &other);

    allocator_type  alloc;
    generator_type  generator;
    unsigned        levels;
    node_type      *head;
    node_type      *tail;
    size_type       item_count;
};

//==============================================================================

#if 1

template <typename Compare, typename T>
inline
bool equivalent(const T &lhs, const T &rhs, const Compare &less)
    { return !less(lhs, rhs) && !less(rhs, lhs); }

template <typename Compare, typename T>
inline
bool less_or_equal(const T &lhs, const T &rhs, const Compare &less)
    { return !less(rhs, lhs); }

#else

// These "simple" versions are left here for efficiency comparison with
// the versions above.
// There should be no appriciable difference in performance (at least, for
// the built-in types).

template <typename Compare, typename T>
inline
bool equivalent(const T &lhs, const T &rhs, Compare &less)
    { return lhs == rhs; }

template <typename Compare, typename T>
inline
bool less_or_equal(const T &lhs, const T &rhs, Compare &less)
    { return lhs <= rhs; }

#endif

} // namespace detail
} // namespace goodliffe

//==============================================================================
#pragma mark - iterators

namespace goodliffe {

template <class T, class C, class A, unsigned NL, class LG, class SLT>
class skip_list<T,C,A,NL,LG,SLT>::iterator
    : public std::iterator<std::bidirectional_iterator_tag,
                           typename skip_list<T,C,A,NL,LG,SLT>::value_type,
                           typename skip_list<T,C,A,NL,LG,SLT>::difference_type,
                           typename skip_list<T,C,A,NL,LG,SLT>::const_pointer,
                           typename skip_list<T,C,A,NL,LG,SLT>::const_reference>
{
public:
    typedef skip_list<T,C,A,NL,LG,SLT>                  parent_type;
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
        { self_type old(*this); node = node->next[0]; return old; }

    self_type &operator--()
        { node = node->prev; return *this; }
    self_type operator--(int) // postdecrement
        { self_type old(*this); node = node->prev; return old; }

    const_reference operator*()  { return node->value; }
    const_pointer   operator->() { return node->value; }
    
    bool operator==(const self_type &other) const
        { return parent == other.parent && node == other.node; }
    bool operator!=(const self_type &other) const
        { return !operator==(other); }
    
    bool operator==(const const_type &other) const
        { return parent == other.parent && node == other.node; }
    bool operator!=(const const_type &other) const
        { return !operator==(other); }

    const parent_type *get_parent() const { return parent; } ///< @internal
    const node_type   *get_node()   const { return node; }   ///< @internal

private:
    friend class const_iterator;
    parent_type *parent;
    node_type   *node;
};

template <class T, class C, class A, unsigned NL, class LG, class SLT>
class skip_list<T,C,A,NL,LG,SLT>::const_iterator
    : public std::iterator<std::bidirectional_iterator_tag,
                           typename skip_list<T,C,A,NL,LG,SLT>::value_type,
                           typename skip_list<T,C,A,NL,LG,SLT>::difference_type,
                           typename skip_list<T,C,A,NL,LG,SLT>::const_pointer,
                           typename skip_list<T,C,A,NL,LG,SLT>::const_reference>
{
public:
    typedef const skip_list<T,C,A,NL,LG,SLT>                    parent_type;
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
        { self_type old(*this); node = node->next[0]; return old; }

    self_type &operator--()
        { node = node->prev; return *this; }
    self_type operator--(int) // postdecrement
        { self_type old(*this); node = node->prev; return old; }

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

    const parent_type *get_parent() const { return parent; } ///< @internal
    const node_type   *get_node()   const { return node; }   ///< @internal

private:
    friend class iterator;
    parent_type *parent;
    node_type   *node;
};

//==============================================================================
#pragma mark - lifetime management

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
skip_list<T,C,A,NL,LG,SLT>::skip_list(const allocator_type &alloc_)
:   impl(alloc_)
{
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
skip_list<T,C,A,NL,LG,SLT>::~skip_list()
{
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
template <class InputIterator>
inline
skip_list<T,C,A,NL,LG,SLT>::skip_list(InputIterator first, InputIterator last, const allocator_type &alloc_)
:   impl(alloc_)
{
    assign(first, last);
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
skip_list<T,C,A,NL,LG,SLT>::skip_list(const skip_list &other)
:   impl(other.get_allocator())
{    
    assign(other.begin(), other.end());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
skip_list<T,C,A,NL,LG,SLT>::skip_list(const skip_list &other, const allocator_type &alloc_)
:   impl(alloc_)
{
    assign(other.begin(), other.end());
}

// C++11
//skip_list(const skip_list &&other);
//skip_list(const skip_list &&other, const Allocator &alloc);
//skip_list(std::initializer_list<T> init, const Allocator &alloc = Allocator());

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
A skip_list<T,C,A,NL,LG,SLT>::get_allocator() const
{
    return impl.get_allocator();
}

//==============================================================================
#pragma mark assignment

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
skip_list<T,C,A,NL,LG,SLT> &
skip_list<T,C,A,NL,LG,SLT>::operator=(const skip_list<T,C,A,NL,LG,SLT> &other)
{
    assign(other.begin(), other.end());
    return *this;
}

//C++11 skip_list& operator=(skip_list&& other);

template <class T, class C, class A, unsigned NL, class LG, class SLT>
template <typename InputIterator>
inline
void skip_list<T,C,A,NL,LG,SLT>::assign(InputIterator first, InputIterator last)
{
    clear();
    while (first != last) insert(*first++);
}

//==============================================================================
#pragma mark element access

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::reference
skip_list<T,C,A,NL,LG,SLT>::front()
{
    assert_that(!empty());
    return impl.front()->value;
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::const_reference
skip_list<T,C,A,NL,LG,SLT>::front() const
{
    assert_that(!empty());
    return impl.front()->value;
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::reference
skip_list<T,C,A,NL,LG,SLT>::back()
{
    assert_that(!empty());
    return impl.one_past_end()->prev->value;
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::const_reference
skip_list<T,C,A,NL,LG,SLT>::back() const
{
    assert_that(!empty());
    return impl.one_past_end()->prev->value;
}

//==============================================================================
#pragma mark iterators

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::iterator
skip_list<T,C,A,NL,LG,SLT>::begin()
{
    return iterator(this, impl.front());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::const_iterator
skip_list<T,C,A,NL,LG,SLT>::begin() const
{
    return const_iterator(this, impl.front());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::const_iterator
skip_list<T,C,A,NL,LG,SLT>::cbegin() const
{
    return const_iterator(this, impl.front());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::iterator
skip_list<T,C,A,NL,LG,SLT>::end()
{
    return iterator(this, impl.one_past_end());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::const_iterator
skip_list<T,C,A,NL,LG,SLT>::end() const
{
    return const_iterator(this, impl.one_past_end());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::const_iterator
skip_list<T,C,A,NL,LG,SLT>::cend() const
{
    return const_iterator(this, impl.one_past_end());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::reverse_iterator
skip_list<T,C,A,NL,LG,SLT>::rbegin()
{
    return reverse_iterator(end());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::const_reverse_iterator
skip_list<T,C,A,NL,LG,SLT>::rbegin() const
{
    return const_reverse_iterator(end());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::const_reverse_iterator
skip_list<T,C,A,NL,LG,SLT>::crbegin() const
{
    return const_reverse_iterator(end());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::reverse_iterator
skip_list<T,C,A,NL,LG,SLT>::rend()
{
    return reverse_iterator(begin());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::const_reverse_iterator
skip_list<T,C,A,NL,LG,SLT>::rend() const
{
    return const_reverse_iterator(begin());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::const_reverse_iterator
skip_list<T,C,A,NL,LG,SLT>::crend() const
{
    return const_reverse_iterator(begin());
}

//==============================================================================
#pragma mark capacity

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
bool skip_list<T,C,A,NL,LG,SLT>::empty() const
{
    return impl.size() == 0;
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::size_type
skip_list<T,C,A,NL,LG,SLT>::size() const
{
    return impl.size();
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::size_type
skip_list<T,C,A,NL,LG,SLT>::max_size() const
{
    return impl.get_allocator().max_size();
}

//==============================================================================
#pragma mark modifiers

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
void skip_list<T,C,A,NL,LG,SLT>::clear()
{
    impl.remove_all();
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::insert_by_value_result
skip_list<T,C,A,NL,LG,SLT>::insert(const value_type &value)
{
    node_type *node = impl.insert(value);
    return std::make_pair(iterator(this, node), impl.is_valid(node));
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::iterator
skip_list<T,C,A,NL,LG,SLT>::insert(const_iterator hint, const value_type &value)
{
    assert_that(hint.get_parent() == this);
    
    const node_type *hint_node = hint.get_node();

    if (impl.is_valid(hint_node) && detail::less_or_equal(value, hint_node->value, impl.less))
        return iterator(this,impl.insert(value)); // bad hint, resort to "normal" insert
    else
        return iterator(this,impl.insert(value,const_cast<node_type*>(hint_node)));
}

//C++11iterator insert const_iterator pos, value_type &&value);

template <class T, class C, class A, unsigned NL, class LG, class SLT>
template <class InputIterator>
inline
void
skip_list<T,C,A,NL,LG,SLT>::insert(InputIterator first, InputIterator last)
{
    iterator last_inserted = end();
    while (first != last)
    {
        last_inserted = insert(last_inserted, *first++);
    }
}

//C++11iterator insert(std::initializer_list<value_type> ilist);
// C++11 emplace

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::size_type
skip_list<T,C,A,NL,LG,SLT>::erase(const value_type &value)
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

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::iterator
skip_list<T,C,A,NL,LG,SLT>::erase(const_iterator position)
{
    assert_that(position.get_parent() == this);
    assert_that(impl.is_valid(position.get_node()));
    node_type *node = const_cast<node_type*>(position.get_node());
    node_type *next = node->next[0];
    impl.remove(node);
    return iterator(this, next);
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::iterator
skip_list<T,C,A,NL,LG,SLT>::erase(const_iterator first, const_iterator last)
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

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
void
skip_list<T,C,A,NL,LG,SLT>::swap(skip_list<T,C,A,NL,LG,SLT> &other)
{
    impl.swap(other.impl);
}
    
//==============================================================================
#pragma mark lookup

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::size_type
skip_list<T,C,A,NL,LG,SLT>::count(const value_type &value) const
{
    const node_type *node = impl.find(value);
    return impl.is_valid(node) && detail::equivalent(node->value, value, impl.less);
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::iterator
skip_list<T,C,A,NL,LG,SLT>::find(const value_type &value)
{
    node_type *node = impl.find(value);
    return impl.is_valid(node) && detail::equivalent(node->value, value, impl.less)
        ? iterator(this, node)
        : end();
}
  
template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
typename skip_list<T,C,A,NL,LG,SLT>::const_iterator
skip_list<T,C,A,NL,LG,SLT>::find(const value_type &value) const
{
    const node_type *node = impl.find(value);
    return impl.is_valid(node) && detail::equivalent(node->value, value, impl.less)
        ? const_iterator(this, node)
        : end();
}

//==============================================================================
#pragma mark - non-members

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
bool operator==(const skip_list<T,C,A,NL,LG,SLT> &lhs, const skip_list<T,C,A,NL,LG,SLT> &rhs)
{
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
bool operator!=(const skip_list<T,C,A,NL,LG,SLT> &lhs, const skip_list<T,C,A,NL,LG,SLT> &rhs)
{
    return !operator==(lhs, rhs);
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
bool operator<(const skip_list<T,C,A,NL,LG,SLT> &lhs, const skip_list<T,C,A,NL,LG,SLT> &rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
bool operator<=(const skip_list<T,C,A,NL,LG,SLT> &lhs, const skip_list<T,C,A,NL,LG,SLT> &rhs)
{
    return !(rhs < lhs);
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
bool operator>(const skip_list<T,C,A,NL,LG,SLT> &lhs, const skip_list<T,C,A,NL,LG,SLT> &rhs)
{
    return rhs < lhs;
}

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
bool operator>=(const skip_list<T,C,A,NL,LG,SLT> &lhs, const skip_list<T,C,A,NL,LG,SLT> &rhs)
{
    return !(lhs < rhs);
}
    
} // namespace goodliffe

template <class T, class C, class A, unsigned NL, class LG, class SLT>
inline
void std::swap(goodliffe::skip_list<T,C,A,NL,LG,SLT> &lhs, goodliffe::skip_list<T,C,A,NL,LG,SLT> &rhs)
{
    lhs.swap(rhs);
}

//==============================================================================
#pragma mark - random_access_skip_list_impl

namespace goodliffe {

template <class T, class C, class A, unsigned NL, class LG>
inline
typename random_access_skip_list<T,C,A,NL,LG>::const_reference
random_access_skip_list<T,C,A,NL,LG>::operator[](unsigned index) const
{
    const node_type *node = impl.at(index);
    assert_that(impl.is_valid(node));
    /*
    // remove this check in release
    static const value_type meh = value_type();
    return impl.is_valid(node) ? node->value : meh;
     */
    return node->value;
}

template <class T, class C, class A, unsigned NL, class LG>
inline
void
random_access_skip_list<T,C,A,NL,LG>::erase_at(size_type index)
{
    node_type *node = impl.at(index);
    assert_that(impl.is_valid(node));
    impl.remove(node);
}

template <class T, class C, class A, unsigned NL, class LG>
inline
typename random_access_skip_list<T,C,A,NL,LG>::iterator
random_access_skip_list<T,C,A,NL,LG>::iterator_at(unsigned index)
{
    node_type *node = impl.at(index);
    return iterator(this, node);
}

template <class T, class C, class A, unsigned NL, class LG>
inline
typename random_access_skip_list<T,C,A,NL,LG>::const_iterator
random_access_skip_list<T,C,A,NL,LG>::iterator_at(unsigned index) const
{
    const node_type *node = impl.at(index);
    return const_iterator(this, node);
}

} // namespace goodliffe

//==============================================================================
#pragma mark - skip_list_node_traits

namespace goodliffe {
namespace detail {

#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
enum
{
    MAGIC_GOOD = 0x01020304,
    MAGIC_BAD  = 0xfefefefe
};
#endif

/// The functions to allocate and deallocate a skip_list_node
template <typename NodeType, typename Allocator>
struct skip_list_node_traits
{
    typedef typename Allocator::template rebind<NodeType>::other  NodeAllocator;
    typedef typename Allocator::template rebind<NodeType*>::other ListAllocator;

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
    static void set_span(NodeType *node, unsigned level, unsigned span) {}
    static void set_all_spans(NodeType *node, unsigned span) {}
    static void decrement_span(NodeType *node, unsigned level) {}
    static unsigned span(const NodeType *node, unsigned level) { return 0; }
};

//==============================================================================

/// Partial specialisation for skip_list_node_with_span
/// (also intialises all spans to zero)
template <typename T, typename Allocator>
struct skip_list_node_traits<skip_list_node_with_span<T>, Allocator>
{
    typedef skip_list_node_with_span<T>                           NodeType;
    typedef typename Allocator::template rebind<NodeType>::other  NodeAllocator;
    typedef typename Allocator::template rebind<NodeType*>::other ListAllocator;
    typedef typename Allocator::template rebind<unsigned>::other  SpanAllocator;

    static
    inline
    NodeType *allocate(unsigned level, Allocator &alloc)
    {
        NodeType *node = NodeAllocator(alloc).allocate(1, (void*)0);
        node->next  = ListAllocator(alloc).allocate(level+1, (void*)0);
        node->span  = SpanAllocator(alloc).allocate(level+1, (void*)0);
        node->level = level;
        for (unsigned n = 0; n <= level; ++n) node->span[n] = 1;
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
        typedef typename Allocator::template rebind<NodeType>::other  NodeAllocator;
        typedef typename Allocator::template rebind<NodeType*>::other ListAllocator;
        typedef typename Allocator::template rebind<unsigned>::other  SpanAllocator;
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
        assert_that(node->magic == MAGIC_GOOD);
        node->magic = MAGIC_BAD;
        for (unsigned n = 0; n <= node->level; ++n) node->next[n] = 0;
        node->prev = 0;
#endif
        SpanAllocator(alloc).deallocate(node->span, node->level+1);
        ListAllocator(alloc).deallocate(node->next, node->level+1);
        NodeAllocator(alloc).deallocate(node, 1);
    }

    static
    void increment_span(NodeType *node, unsigned level)
    {
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
        assert_that(node->level < level);
#endif
        
        // Level 0 always has a span of 1
        if (level)
            ++(node->span[level]);
    }
    static
    void set_span(NodeType *node, unsigned level, unsigned span)
    {
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
        assert_that(node->level < level);
#endif

        if (level)
            node->span[level] = span;
    }
    static
    void set_all_spans(NodeType *node, unsigned span)
    {
        for (unsigned n = 0; n < node->level; ++n)
            node->span[n] = span;
    }
    static
    void decrement_span(NodeType *node, unsigned level)
    {
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
        assert_that(node->level < level);
#endif
        if (level)
            --(node->span[level]);
    }
    static
    inline
    unsigned span(const NodeType *node, unsigned level)
    {
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
        assert_that(node->level < level);
#endif
        return node->span[level];
    }
};

//==============================================================================
#pragma mark - skip_list_impl

template <class T, class C, class A, unsigned NL, class LG, class N>
inline
skip_list_impl<T,C,A,NL,LG,N>::skip_list_impl(const allocator_type &alloc_)
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
    }
    head->prev = 0;
    tail->prev = head;
}

template <class T, class C, class A, unsigned NL, class LG, class N>
inline
skip_list_impl<T,C,A,NL,LG,N>::~skip_list_impl()
{
    remove_all();
    node_traits::deallocate(head, alloc);
    node_traits::deallocate(tail, alloc);
}

template <class T, class C, class A, unsigned NL, class LG, class N>
inline
A skip_list_impl<T,C,A,NL,LG,N>::get_allocator() const
{
    return alloc;
}

template <class T, class C, class A, unsigned NL, class LG, class N>
inline
typename skip_list_impl<T,C,A,NL,LG,N>::node_type *
skip_list_impl<T,C,A,NL,LG,N>::find(const value_type &value) const
{
    // I could have a const and non-const overload, but this is simpler
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

template <class T, class C, class A, unsigned NL, class LG, class N>
inline
typename skip_list_impl<T,C,A,NL,LG,N>::node_type *
skip_list_impl<T,C,A,NL,LG,N>::at(size_type index)
{
    // only compiles for node_type where "node->span" is valid
    static_assert_that(sizeof(node_type) == sizeof(skip_list_node_with_span<T>));

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

template <class T, class C, class A, unsigned NL, class LG, class N>
inline
const typename skip_list_impl<T,C,A,NL,LG,N>::node_type *
skip_list_impl<T,C,A,NL,LG,N>::at(size_type index) const
{
    // only compiles for node_type where "node->span" is valid
    static_assert_that(sizeof(node_type) == sizeof(skip_list_node_with_span<T>));

    unsigned l = levels;
    const node_type *node = head;
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

// TODO: Hint is now ignored (has to be, for spans to work)
template <class T, class C, class A, unsigned NL, class LG, class N>
inline
typename skip_list_impl<T,C,A,NL,LG,N>::node_type*
skip_list_impl<T,C,A,NL,LG,N>::insert(const value_type &value, node_type *hint)
{
    const unsigned level = new_level();

    node_type *new_node = node_traits::allocate(level, alloc);
    assert_that(new_node);
    assert_that(new_node->level == level);
    alloc.construct(&new_node->value, value);
    
    node_type *chain[num_levels] = {0};
    size_type  indexes[num_levels] = {0};
    size_type  index = 0;

    {
        node_type *cur = head;
        unsigned l = num_levels;
        while (l)
        {
            --l;
            assert_that(l <= cur->level);
            while (cur->next[l] != tail && less(cur->next[l]->value, value))
            {
                index += node_traits::span(cur, l);
                cur = cur->next[l];
                assert_that(l <= cur->level);
            }
            chain[l]   = cur;
            indexes[l] = index;
        }
    }

    // Do not allow repeated values in the list (we could in a "multi_skip_list")
    {
        node_type *next = chain[0]->next[0];
        if (next != tail && detail::equivalent(next->value, value, less))
            return tail;
    }

    //fprintf(stderr, "Inserting index=%lu\n", index);
    //for (unsigned l = 0; l < levels; ++l)
    //    fprintf(stderr, "  ch[%u]=%lu\n", l, indexes[l]);

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
            unsigned prev_span = node_traits::span(chain[l], l);
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

template <class T, class C, class A, unsigned NL, class LG, class N>
inline
void
skip_list_impl<T,C,A,NL,LG,N>::remove(node_type *node)
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
            node_traits::decrement_span(cur, l);
            cur->next[l] = node->next[l];
        }
    }

    alloc.destroy(&node->value);
    node_traits::deallocate(node, alloc);

    item_count--;
    
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    check();
#endif
}

template <class T, class C, class A, unsigned NL, class LG, class N>
inline
void
skip_list_impl<T,C,A,NL,LG,N>::remove_all()
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

template <class T, class C, class A, unsigned NL, class LG, class N>
inline
void 
skip_list_impl<T,C,A,NL,LG,N>::remove_between(node_type *first, node_type *last)
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
            // TODO: span
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
        node_traits::deallocate(first, alloc);
        item_count--;
        first = next;
    }
        
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    check();
#endif
}

template <class T, class C, class A, unsigned NL, class LG, class N>
inline
unsigned skip_list_impl<T,C,A,NL,LG,N>::new_level()
{    
    unsigned level = generator.new_level();
    if (level >= levels)
    {
        level = levels;
        ++levels;
    }
    return level;
}

template <class T, class C, class A, unsigned NL, class LG, class N>
inline
void skip_list_impl<T,C,A,NL,LG,N>::swap(skip_list_impl &other)
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
template <class T, class C, class A, unsigned NL, class LG, class N>
template <class STREAM>
inline
void skip_list_impl<T,C,A,NL,LG,N>::dump(STREAM &s) const
{
    s << "skip_list(size="<<item_count<<",levels=" << levels << ")\n";
    for (unsigned l = 0; l < levels+1; ++l)
    {
        s << "  [" << l << "]" ;
        const node_type *n = head;
        while (n)
        {
            assert_that(l <= n->level);
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
                s << node_traits::span(n, l) << ">"
                  << " "
                  << (prev_ok?"<":"X");
            }
            n = next;
        }
        s << "\n";
    }
}

#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
// for diagnostics only
template <class T, class C, class A, unsigned NL, class LG, class N>
inline
bool skip_list_impl<T,C,A,NL,LG,N>::check() const
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
                return false;
            }

            // if level 0, we check prev pointers
            if (l == 0 && n->next[l]->prev != n)
            {
                assert_that(false && "chain error");
                return false;
            }
            // check values are in order
            node_type *next = n->next[l];
            if (n != head && next != tail)
            {
                if (!(less(n->value, next->value)))
                {
                    assert_that(false && "value order error");
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
            return false;
        }
    }
    return true;
}
#endif

//==============================================================================
#pragma mark - skip_list_level_generator

template <unsigned ML>
inline
unsigned bit_based_skip_list_level_generator<ML>::new_level()
{
    // The number of 1-bits before we encounter the first 0-bit is the level of
    /// the node. Since R is 32-bit, the level can be at most 32.
    assert_that(num_levels < 33);

    unsigned level = 0;
    for (unsigned number = unsigned(rand()); (number & 1) == 1; number >>= 1)
    {
        level++;
    }
    return level;
}

template <unsigned ML>
inline
unsigned skip_list_level_generator<ML>::new_level()
{
    float f = float(std::rand())/float(RAND_MAX);
    unsigned level = unsigned(std::log(f)/std::log(0.5));
    return level < num_levels ? level : num_levels;
}

} // namespace detail
} // namespace goodliffe

//==============================================================================

#ifdef _MSC_VER
#pragma warning( pop )
#endif
