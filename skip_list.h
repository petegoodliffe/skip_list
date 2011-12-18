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
    template <unsigned NumLevels>   class bit_based_skip_list_level_generator;
    template <unsigned NumLevels>   class skip_list_level_generator;
    template <typename T>           struct skip_list_node;
    
    template <typename T,typename C,typename A,unsigned NL,typename LG,typename N>
    class skip_list_impl;

    template <typename LIST> class skip_list_iterator;
    template <typename LIST> class skip_list_const_iterator;
    template <typename LIST> class random_access_skip_list_iterator;
    template <typename LIST> class random_access_skip_list_const_iterator;
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
          typename SkipListType   = typename detail::skip_list_impl<T,Compare,Allocator,NumLevels,LevelGenerator,detail::skip_list_node<T> >,
          template <typename> class Iterator = detail::skip_list_iterator >
class skip_list
{
protected:
    typedef skip_list
        <
            T,Compare,Allocator,NumLevels,LevelGenerator,SkipListType,Iterator
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

    typedef Iterator<self_type>                         iterator;
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

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
bool operator==(const skip_list<T,C,A,NL,LG,SLT,I> &lhs, const skip_list<T,C,A,NL,LG,SLT,I> &rhs);

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
bool operator!=(const skip_list<T,C,A,NL,LG,SLT,I> &lhs, const skip_list<T,C,A,NL,LG,SLT,I> &rhs);

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
bool operator<(const skip_list<T,C,A,NL,LG,SLT,I> &lhs, const skip_list<T,C,A,NL,LG,SLT,I> &rhs);

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
bool operator<=(const skip_list<T,C,A,NL,LG,SLT,I> &lhs, const skip_list<T,C,A,NL,LG,SLT,I> &rhs);

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
bool operator>(const skip_list<T,C,A,NL,LG,SLT,I> &lhs, const skip_list<T,C,A,NL,LG,SLT,I> &rhs);

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
bool operator>=(const skip_list<T,C,A,NL,LG,SLT,I> &lhs, const skip_list<T,C,A,NL,LG,SLT,I> &rhs);
    
} // namespace goodliffe

namespace std
{
    template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
    void swap(goodliffe::skip_list<T,C,A,NL,LG,SLT,I> &lhs, goodliffe::skip_list<T,C,A,NL,LG,SLT,I> &rhs);
}

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
    
    typedef T                                   value_type;
    typedef typename Allocator::size_type       size_type;
    typedef typename Allocator::difference_type difference_type;
    typedef Allocator                           allocator_type;
    typedef Compare                             compare_type;
    typedef LevelGenerator                      generator_type;
    typedef NodeType                            node_type;

    static const unsigned num_levels = NumLevels;

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
namespace detail {

template <typename SKIP_LIST>
class skip_list_iterator
    : public std::iterator<std::bidirectional_iterator_tag,
                           typename SKIP_LIST::value_type,
                           typename SKIP_LIST::difference_type,
                           typename SKIP_LIST::const_pointer,
                           typename SKIP_LIST::const_reference>
{
public:
    typedef SKIP_LIST                                   parent_type;
    typedef skip_list_const_iterator<SKIP_LIST>         const_iterator;
    typedef typename parent_type::impl_type::node_type  node_type;
    typedef skip_list_iterator<SKIP_LIST>               self_type;

    skip_list_iterator()
        : parent(0), node(0) {}
    skip_list_iterator(parent_type *parent_, node_type *node_)
        : parent(parent_), node(node_) {}

    self_type &operator++()
        { node = node->next[0]; return *this; }
    self_type operator++(int) // postincrement
        { self_type old(*this); node = node->next[0]; return old; }

    self_type &operator--()
        { node = node->prev; return *this; }
    self_type operator--(int) // postdecrement
        { self_type old(*this); node = node->prev; return old; }

    typename parent_type::const_reference operator*()  { return node->value; }
    typename parent_type::const_pointer   operator->() { return node->value; }
    
    bool operator==(const self_type &other) const
        { return parent == other.parent && node == other.node; }
    bool operator!=(const self_type &other) const
        { return !operator==(other); }
    
    bool operator==(const const_iterator &other) const
        { return parent == other.parent && node == other.node; }
    bool operator!=(const const_iterator &other) const
        { return !operator==(other); }

    const parent_type *get_parent() const { return parent; } ///< @internal
    const node_type   *get_node()   const { return node; }   ///< @internal

private:
    friend class skip_list_const_iterator<SKIP_LIST>;
    parent_type *parent;
    node_type   *node;
};

template <class SKIP_LIST>
class skip_list_const_iterator
    : public std::iterator<std::bidirectional_iterator_tag,
                           typename SKIP_LIST::value_type,
                           typename SKIP_LIST::difference_type,
                           typename SKIP_LIST::const_pointer,
                           typename SKIP_LIST::const_reference>
{
public:
    typedef const SKIP_LIST                                     parent_type;
    typedef skip_list_iterator<SKIP_LIST>                       iterator;
    typedef const typename parent_type::impl_type::node_type    node_type;
    typedef skip_list_const_iterator<SKIP_LIST>                 self_type;

    skip_list_const_iterator()
        : parent(0), node(0) {}
    skip_list_const_iterator(const iterator &i)
        : parent(i.parent), node(i.node) {}
    skip_list_const_iterator(const parent_type *parent_, node_type *node_)
        : parent(parent_), node(node_) {}

    self_type &operator++()
        { node = node->next[0]; return *this; }
    self_type operator++(int) // postincrement
        { self_type old(*this); node = node->next[0]; return old; }

    self_type &operator--()
        { node = node->prev; return *this; }
    self_type operator--(int) // postdecrement
        { self_type old(*this); node = node->prev; return old; }

    typename parent_type::const_reference operator*()  { return node->value; }
    typename parent_type::const_pointer   operator->() { return node->value; }

    bool operator==(const self_type &other) const
        { return parent == other.parent && node == other.node; }
    bool operator!=(const self_type &other) const
        { return !operator==(other); }

    bool operator==(const iterator &other) const
        { return parent == other.parent && node == other.node; }
    bool operator!=(const iterator &other) const
        { return !operator==(other); }

    const parent_type *get_parent() const { return parent; } ///< @internal
    const node_type   *get_node()   const { return node; }   ///< @internal

private:
    friend class skip_list_iterator<SKIP_LIST>;
    parent_type *parent;
    node_type   *node;
};

} // namespace detail

//==============================================================================
#pragma mark - lifetime management

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
skip_list<T,C,A,NL,LG,SLT,I>::skip_list(const allocator_type &alloc_)
:   impl(alloc_)
{
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
skip_list<T,C,A,NL,LG,SLT,I>::~skip_list()
{
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
template <class InputIterator>
inline
skip_list<T,C,A,NL,LG,SLT,I>::skip_list(InputIterator first, InputIterator last, const allocator_type &alloc_)
:   impl(alloc_)
{
    assign(first, last);
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
skip_list<T,C,A,NL,LG,SLT,I>::skip_list(const skip_list &other)
:   impl(other.get_allocator())
{    
    assign(other.begin(), other.end());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
skip_list<T,C,A,NL,LG,SLT,I>::skip_list(const skip_list &other, const allocator_type &alloc_)
:   impl(alloc_)
{
    assign(other.begin(), other.end());
}

// C++11
//skip_list(const skip_list &&other);
//skip_list(const skip_list &&other, const Allocator &alloc);
//skip_list(std::initializer_list<T> init, const Allocator &alloc = Allocator());

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
A skip_list<T,C,A,NL,LG,SLT,I>::get_allocator() const
{
    return impl.get_allocator();
}

//==============================================================================
#pragma mark assignment

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
skip_list<T,C,A,NL,LG,SLT,I> &
skip_list<T,C,A,NL,LG,SLT,I>::operator=(const skip_list<T,C,A,NL,LG,SLT,I> &other)
{
    assign(other.begin(), other.end());
    return *this;
}

//C++11 skip_list& operator=(skip_list&& other);

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
template <typename InputIterator>
inline
void skip_list<T,C,A,NL,LG,SLT,I>::assign(InputIterator first, InputIterator last)
{
    clear();
    while (first != last) insert(*first++);
}

//==============================================================================
#pragma mark element access

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::reference
skip_list<T,C,A,NL,LG,SLT,I>::front()
{
    assert_that(!empty());
    return impl.front()->value;
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::const_reference
skip_list<T,C,A,NL,LG,SLT,I>::front() const
{
    assert_that(!empty());
    return impl.front()->value;
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::reference
skip_list<T,C,A,NL,LG,SLT,I>::back()
{
    assert_that(!empty());
    return impl.one_past_end()->prev->value;
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::const_reference
skip_list<T,C,A,NL,LG,SLT,I>::back() const
{
    assert_that(!empty());
    return impl.one_past_end()->prev->value;
}

//==============================================================================
#pragma mark iterators

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::iterator
skip_list<T,C,A,NL,LG,SLT,I>::begin()
{
    return iterator(this, impl.front());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::const_iterator
skip_list<T,C,A,NL,LG,SLT,I>::begin() const
{
    return const_iterator(this, impl.front());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::const_iterator
skip_list<T,C,A,NL,LG,SLT,I>::cbegin() const
{
    return const_iterator(this, impl.front());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::iterator
skip_list<T,C,A,NL,LG,SLT,I>::end()
{
    return iterator(this, impl.one_past_end());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::const_iterator
skip_list<T,C,A,NL,LG,SLT,I>::end() const
{
    return const_iterator(this, impl.one_past_end());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::const_iterator
skip_list<T,C,A,NL,LG,SLT,I>::cend() const
{
    return const_iterator(this, impl.one_past_end());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::reverse_iterator
skip_list<T,C,A,NL,LG,SLT,I>::rbegin()
{
    return reverse_iterator(end());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::const_reverse_iterator
skip_list<T,C,A,NL,LG,SLT,I>::rbegin() const
{
    return const_reverse_iterator(end());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::const_reverse_iterator
skip_list<T,C,A,NL,LG,SLT,I>::crbegin() const
{
    return const_reverse_iterator(end());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::reverse_iterator
skip_list<T,C,A,NL,LG,SLT,I>::rend()
{
    return reverse_iterator(begin());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::const_reverse_iterator
skip_list<T,C,A,NL,LG,SLT,I>::rend() const
{
    return const_reverse_iterator(begin());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::const_reverse_iterator
skip_list<T,C,A,NL,LG,SLT,I>::crend() const
{
    return const_reverse_iterator(begin());
}

//==============================================================================
#pragma mark capacity

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
bool skip_list<T,C,A,NL,LG,SLT,I>::empty() const
{
    return impl.size() == 0;
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::size_type
skip_list<T,C,A,NL,LG,SLT,I>::size() const
{
    return impl.size();
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::size_type
skip_list<T,C,A,NL,LG,SLT,I>::max_size() const
{
    return impl.get_allocator().max_size();
}

//==============================================================================
#pragma mark modifiers

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
void skip_list<T,C,A,NL,LG,SLT,I>::clear()
{
    impl.remove_all();
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::insert_by_value_result
skip_list<T,C,A,NL,LG,SLT,I>::insert(const value_type &value)
{
    node_type *node = impl.insert(value);
    return std::make_pair(iterator(this, node), impl.is_valid(node));
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::iterator
skip_list<T,C,A,NL,LG,SLT,I>::insert(const_iterator hint, const value_type &value)
{
    assert_that(hint.get_parent() == this);
    
    const node_type *hint_node = hint.get_node();

    if (impl.is_valid(hint_node) && detail::less_or_equal(value, hint_node->value, impl.less))
        return iterator(this,impl.insert(value)); // bad hint, resort to "normal" insert
    else
        return iterator(this,impl.insert(value,const_cast<node_type*>(hint_node)));
}

//C++11iterator insert const_iterator pos, value_type &&value);

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
template <class InputIterator>
inline
void
skip_list<T,C,A,NL,LG,SLT,I>::insert(InputIterator first, InputIterator last)
{
    iterator last_inserted = end();
    while (first != last)
    {
        last_inserted = insert(last_inserted, *first++);
    }
}

//C++11iterator insert(std::initializer_list<value_type> ilist);
// C++11 emplace

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::size_type
skip_list<T,C,A,NL,LG,SLT,I>::erase(const value_type &value)
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

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::iterator
skip_list<T,C,A,NL,LG,SLT,I>::erase(const_iterator position)
{
    assert_that(position.get_parent() == this);
    assert_that(impl.is_valid(position.get_node()));
    node_type *node = const_cast<node_type*>(position.get_node());
    node_type *next = node->next[0];
    impl.remove(node);
    return iterator(this, next);
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::iterator
skip_list<T,C,A,NL,LG,SLT,I>::erase(const_iterator first, const_iterator last)
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

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
void
skip_list<T,C,A,NL,LG,SLT,I>::swap(skip_list<T,C,A,NL,LG,SLT,I> &other)
{
    impl.swap(other.impl);
}
    
//==============================================================================
#pragma mark lookup

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::size_type
skip_list<T,C,A,NL,LG,SLT,I>::count(const value_type &value) const
{
    const node_type *node = impl.find(value);
    return impl.is_valid(node) && detail::equivalent(node->value, value, impl.less);
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::iterator
skip_list<T,C,A,NL,LG,SLT,I>::find(const value_type &value)
{
    node_type *node = impl.find(value);
    return impl.is_valid(node) && detail::equivalent(node->value, value, impl.less)
        ? iterator(this, node)
        : end();
}
  
template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
typename skip_list<T,C,A,NL,LG,SLT,I>::const_iterator
skip_list<T,C,A,NL,LG,SLT,I>::find(const value_type &value) const
{
    const node_type *node = impl.find(value);
    return impl.is_valid(node) && detail::equivalent(node->value, value, impl.less)
        ? const_iterator(this, node)
        : end();
}

//==============================================================================
#pragma mark - non-members

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
bool operator==(const skip_list<T,C,A,NL,LG,SLT,I> &lhs, const skip_list<T,C,A,NL,LG,SLT,I> &rhs)
{
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
bool operator!=(const skip_list<T,C,A,NL,LG,SLT,I> &lhs, const skip_list<T,C,A,NL,LG,SLT,I> &rhs)
{
    return !operator==(lhs, rhs);
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
bool operator<(const skip_list<T,C,A,NL,LG,SLT,I> &lhs, const skip_list<T,C,A,NL,LG,SLT,I> &rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
bool operator<=(const skip_list<T,C,A,NL,LG,SLT,I> &lhs, const skip_list<T,C,A,NL,LG,SLT,I> &rhs)
{
    return !(rhs < lhs);
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
bool operator>(const skip_list<T,C,A,NL,LG,SLT,I> &lhs, const skip_list<T,C,A,NL,LG,SLT,I> &rhs)
{
    return rhs < lhs;
}

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
bool operator>=(const skip_list<T,C,A,NL,LG,SLT,I> &lhs, const skip_list<T,C,A,NL,LG,SLT,I> &rhs)
{
    return !(lhs < rhs);
}
    
} // namespace goodliffe

template <class T, class C, class A, unsigned NL, class LG, class SLT, template <class> class I>
inline
void std::swap(goodliffe::skip_list<T,C,A,NL,LG,SLT,I> &lhs, goodliffe::skip_list<T,C,A,NL,LG,SLT,I> &rhs)
{
    lhs.swap(rhs);
}

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
        node_traits::set_span(head, n, 1);
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

template <class T, class C, class A, unsigned NL, class LG, class N>
inline
typename skip_list_impl<T,C,A,NL,LG,N>::size_type
skip_list_impl<T,C,A,NL,LG,N>::find_chain(const value_type &value, node_type **chain, size_type *indexes) const
{
    size_type index = 0;
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
    
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    for (unsigned l1 = 0; l1 < num_levels; ++l1)
    {
        assert_that(chain[l1]->level >= l1);
    }
#endif

    return index;
}

// TODO: fold these down
template <class T, class C, class A, unsigned NL, class LG, class N>
inline
typename skip_list_impl<T,C,A,NL,LG,N>::size_type
skip_list_impl<T,C,A,NL,LG,N>::find_chain(const node_type *node, node_type **chain, size_type *indexes) const
{
    assert_that(is_valid(node));
    size_type index = 0;
    node_type *cur = head;
    unsigned l = num_levels;
    while (l)
    {
        --l;
        assert_that(l <= cur->level);
        while (cur->next[l] != tail && less(cur->next[l]->value, node->value))
        {
            index += node_traits::span(cur, l);
            cur = cur->next[l];
            assert_that(l <= cur->level);
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
template <class T, class C, class A, unsigned NL, class LG, class N>
inline
typename skip_list_impl<T,C,A,NL,LG,N>::size_type
skip_list_impl<T,C,A,NL,LG,N>::find_end_chain(node_type **chain, size_type *indexes) const
{
    size_type index = 0;
    node_type *cur = head;
    unsigned l = num_levels;
    while (l)
    {
        --l;
        assert_that(l <= cur->level);
        while (cur->next[l] != tail)
        {
            index += node_traits::span(cur, l);
            cur = cur->next[l];
            assert_that(l <= cur->level);
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

template <class T, class C, class A, unsigned NL, class LG, class N>
inline
void
skip_list_impl<T,C,A,NL,LG,N>::remove(node_type *node)
{
    assert_that(is_valid(node));
    assert_that(node->next[0]);
    
    node_type *chain[num_levels]   = {0};
    size_type  indexes[num_levels] = {0};
    find_chain(node, chain, indexes);

    node->next[0]->prev = node->prev;
    
    for (unsigned l = 0; l < num_levels; ++l)
    {
        if (l <= node->level)
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
    assert_that(is_valid(first));
    assert_that(is_valid(last));

    node_type * const prev         = first->prev;
    node_type * const one_past_end = last->next[0];

    node_type *first_chain[num_levels]   = {0};
    node_type *last_chain[num_levels]    = {0};
    size_type  first_indexes[num_levels] = {0};// TODO no indexes needed
    size_type  last_indexes[num_levels]  = {0};// TODO no indexes needed
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
    // backwards pointer
    one_past_end->prev = prev;

    // forwards pointers (and spans)
    for (unsigned l = 0; l < num_levels; ++l)
    {
        // forwards pointer
        if (l <= last->level)
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
                if (next != tail && !(n->value < next->value))
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
