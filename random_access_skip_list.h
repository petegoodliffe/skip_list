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
                    detail::skip_list_node_with_span<T, typename Allocator::size_type>
                >
        >
        parent_type;
    typedef random_access_skip_list
        <
            T,Compare,Allocator,NumLevels,LevelGenerator
        >
        self_type;

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

#ifdef _MSC_VER
#pragma warning( pop )
#endif
