//==============================================================================
// random_access_skip_list.h
// Copyright (c) 2011 Pete Goodliffe. All rights reserved.
//==============================================================================

#pragma once

#include "skip_list.h"

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
    template <typename T, typename SPAN> struct skip_list_node_with_span;
    template <typename LIST> class random_access_skip_list_iterator;
    template <typename LIST> class random_access_skip_list_const_iterator;
}
}

//==============================================================================
#pragma mark - random_access_skip_list

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
                    detail::skip_list_node_with_span<T,typename Allocator::size_type>
                >,
            detail::random_access_skip_list_iterator
        >
{
private:
    typedef skip_list
        <
            T, Compare, Allocator, NumLevels, LevelGenerator,
            typename detail::skip_list_impl
                <
                    T,Compare,Allocator,NumLevels,LevelGenerator,
                    detail::skip_list_node_with_span<T, typename Allocator::size_type>
                >,
            detail::random_access_skip_list_iterator
        >
        parent_type;
    typedef random_access_skip_list
        <
            T,Compare,Allocator,NumLevels,LevelGenerator
        >
        self_type;
    
    template <typename T1>
    friend class detail::random_access_skip_list_iterator;
    template <typename T1>
    friend class detail::random_access_skip_list_const_iterator;

protected:
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
    
    size_type index_of(const const_iterator &i) const;

    void erase_at(size_type index);
};

} // namespace goodliffe

//==============================================================================
#pragma mark - iterators

namespace goodliffe {
namespace detail {

template <typename SKIP_LIST>
class random_access_skip_list_iterator
    : public std::iterator<std::bidirectional_iterator_tag, // TODO random access
                           typename SKIP_LIST::value_type,
                           typename SKIP_LIST::difference_type,
                           typename SKIP_LIST::const_pointer,
                           typename SKIP_LIST::const_reference>
{
public:
    typedef SKIP_LIST                                           parent_type;
    typedef random_access_skip_list_const_iterator<SKIP_LIST>   const_iterator;
    typedef typename parent_type::impl_type::node_type          node_type;
    typedef random_access_skip_list_iterator<SKIP_LIST>         self_type;

    random_access_skip_list_iterator()
        : parent(0), node(0) {}
    random_access_skip_list_iterator(parent_type *parent_, node_type *node_)
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

template <typename SKIP_LIST>
class random_access_skip_list_const_iterator
    : public std::iterator<std::bidirectional_iterator_tag,
                           typename SKIP_LIST::value_type,
                           typename SKIP_LIST::difference_type,
                           typename SKIP_LIST::const_pointer,
                           typename SKIP_LIST::const_reference>
{
public:
    typedef const SKIP_LIST                                     parent_type;
    typedef random_access_skip_list_iterator<SKIP_LIST>         normal_iterator;
    typedef const typename parent_type::impl_type::node_type    node_type;
    typedef random_access_skip_list_const_iterator<SKIP_LIST>   self_type;

    random_access_skip_list_const_iterator()
        : parent(0), node(0) {}
    random_access_skip_list_const_iterator(const normal_iterator &i)
        : parent(i.get_parent()), node(i.get_node()) {}
    random_access_skip_list_const_iterator(const parent_type *parent_, node_type *node_)
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

    bool operator==(const normal_iterator &other) const
        { return parent == other.parent && node == other.node; }
    bool operator!=(const normal_iterator &other) const
        { return !operator==(other); }

    const parent_type *get_parent() const { return parent; } ///< @internal
    const node_type   *get_node()   const { return node; }   ///< @internal

private:
    parent_type *parent;
    node_type   *node;
};

} // namespace detail
} // namespace goodliffe

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

template <class T, class C, class A, unsigned NL, class LG>
inline
typename random_access_skip_list<T,C,A,NL,LG>::size_type
random_access_skip_list<T,C,A,NL,LG>::index_of(const const_iterator &i) const
{
    return impl.index_of(i.get_node());
}

} // namespace goodliffe

//==============================================================================
#pragma mark - skip_list_impl declaration

namespace goodliffe {
namespace detail {

template <typename T, typename SPAN>
struct skip_list_node_with_span
{
    typedef skip_list_node_with_span<T,SPAN> self_type;
    typedef SPAN                             size_type;
    
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    unsigned    magic;
#endif
    T           value;
    unsigned    level;
    self_type  *prev;
    self_type **next; ///< effectively node_type *next[level+1];
    size_type  *span; ///< effectively unsigned span[level+1];
};

}
}

//==============================================================================
#pragma mark skip_list_node_traits

namespace goodliffe {
namespace detail {

/// Partial specialisation for skip_list_node_with_span
/// (also intialises all spans to zero)
template <typename T, typename Allocator>
struct skip_list_node_traits<skip_list_node_with_span<T,typename Allocator::size_type>, Allocator>
{
    typedef typename Allocator::size_type                           Span;
    typedef skip_list_node_with_span<T,Span>                        NodeType;
    typedef typename Allocator::template rebind<NodeType>::other    NodeAllocator;
    typedef typename Allocator::template rebind<NodeType*>::other   ListAllocator;
    typedef typename Allocator::template rebind<Span>::other        SpanAllocator;
    typedef typename Allocator::difference_type                     Difference;
    typedef typename Allocator::size_type                           Size;

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
        assert_that(level <= node->level);
#endif
        
        // Level 0 always has a span of 1
        if (level)
            ++(node->span[level]);
    }
    static
    void change_span(NodeType *node, unsigned level, Difference by)
    {
        // Level 0 always has a span of 1
        if (level)
            node->span[level] += by;
    }
    static
    void set_span(NodeType *node, unsigned level, Size span)
    {
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
        assert_that(level <= node->level);
#endif

        if (level)
            node->span[level] = span;
    }
    static
    void set_all_spans(NodeType *node, Size span)
    {
        for (unsigned n = 0; n < node->level; ++n)
            node->span[n] = span;
    }
    static
    void decrement_span(NodeType *node, unsigned level)
    {
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
        assert_that(level <= node->level);
#endif
        if (level)
            --(node->span[level]);
    }
    static
    inline
    Size span(const NodeType *node, unsigned level)
    {
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
        assert_that(level <= node->level);
#endif
        return node->span[level];
    }
};

}
}

//==============================================================================
#pragma mark skip_list_impl

namespace goodliffe {
namespace detail {

template <class T, class C, class A, unsigned NL, class LG, class N>
inline
typename skip_list_impl<T,C,A,NL,LG,N>::node_type *
skip_list_impl<T,C,A,NL,LG,N>::at(size_type index)
{
    // only compiles for node_type where "node->span" is valid
    static_assert_that(sizeof(node_type) == sizeof(skip_list_node_with_span<T,size_type>));

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
    static_assert_that(sizeof(node_type) == sizeof(skip_list_node_with_span<T,size_type>));

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

template <class T, class C, class A, unsigned NL, class LG, class N>
inline
typename skip_list_impl<T,C,A,NL,LG,N>::size_type
skip_list_impl<T,C,A,NL,LG,N>::index_of(const node_type *node) const
{
    // only compiles for node_type where "node->span" is valid
    static_assert_that(sizeof(node_type) == sizeof(skip_list_node_with_span<T,size_type>));

    node_type *chain[num_levels]   = {0};
    size_type  indexes[num_levels] = {0};
    return find_chain(node, chain, indexes);
}

}
}

//==============================================================================

#ifdef _MSC_VER
#pragma warning( pop )
#endif
