//============================================================================
// tests.cpp
// Copyright (c) 2011 Pete Goodliffe. All rights reserved
//============================================================================

// MSVS quietens warnings
#pragma warning (disable : 4068 ) /* disable unknown pragma warnings */

#include "skip_list.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using goodliffe::skip_list;
using goodliffe::detail::skip_list_impl;

TEST_CASE( "skip_list/smoketest", "" )
{
    //REQUIRE(false);
}

//============================================================================

struct Struct
{
    int i;
    float f;

    Struct() : i(0), f(0) {}
    Struct(int i_, float f_) : i(i_), f(f_) {}
};

inline
bool operator==(const Struct &lhs, const Struct &rhs)
    { return lhs.i == rhs.i && lhs.f == rhs.f; }
inline
bool operator!=(const Struct &lhs, const Struct &rhs)
    { return !operator==(lhs,rhs); }

//============================================================================

template <typename T = int>
struct TestingAllocator : Struct
{
    TestingAllocator() : Struct() {}
    template <class OTHER>
    TestingAllocator(OTHER &) : Struct() {}
    TestingAllocator(int i, float f) : Struct(i,f) {}

    template <typename OTHER>
    struct rebind { typedef TestingAllocator<OTHER> other; };
    
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    
    pointer allocate(size_type n, std::allocator<void>::const_pointer hint=0)
        { return (pointer)new char[sizeof(T)*n]; }
    void deallocate(pointer p, size_type n)
        { delete [] p; }
    void construct(pointer p, const_reference val)
        { new ((void*)p) T (val); }
    void destroy(pointer p)
        { ((T*)p)->~T(); }
};

//============================================================================
// simple construction tests

TEST_CASE( "skip_list/can be constructed and destroyed", "" )
{
    skip_list<int>    sl_int;
    skip_list<float>  sl_float;
    skip_list<Struct> sl_struct;

    REQUIRE(true);
}

TEST_CASE( "skip_list/default construction gets default allocator", "" )
{
    REQUIRE(skip_list<int>().get_allocator() == std::allocator<int>());
    REQUIRE((skip_list<int,std::less<int>,TestingAllocator<int> >().get_allocator()) == TestingAllocator<int>());
}

TEST_CASE( "skip_list/construction with allocator returns copy of that allocator", "" )
{
    REQUIRE((skip_list<int,std::less<int>,TestingAllocator<int> >(TestingAllocator<int>(10,4)).get_allocator()) == TestingAllocator<int>(10,4));
}

TEST_CASE( "skip_list/constructed list returns empty()", "" )
{
    const skip_list<int> list;
    REQUIRE(list.empty());
}

TEST_CASE( "skip_list/max_size() does something sensible", "" )
{
    const skip_list<int> list;
    REQUIRE(list.max_size() > 10000);
}

TEST_CASE( "skip_list/empty list does not count() a value", "" )
{
    const skip_list<int> list;
    REQUIRE(list.count(10) == 0);
    REQUIRE(list.count(1) == 0);
    REQUIRE(list.count(0) == 0);
}

TEST_CASE( "skip_list/empty list does not contains() a value", "" )
{
    const skip_list<int> list;
    REQUIRE_FALSE(list.contains(10));
    REQUIRE_FALSE(list.contains(1));
    REQUIRE_FALSE(list.contains(0));
}

//============================================================================
// iterators

TEST_CASE( "skip_list/default iterator equality", "" )
{
    skip_list<int>::iterator a, b;
    REQUIRE(a == b);
}

TEST_CASE( "skip_list/default iterator inequality", "" )
{
    skip_list<int>::iterator a, b;
    REQUIRE_FALSE(a != b);
}

TEST_CASE( "skip_list/default const_iterator equality", "" )
{
    skip_list<int>::const_iterator a, b;
    REQUIRE(a == b);
}

TEST_CASE( "skip_list/default const_iterator inequality", "" )
{
    skip_list<int>::const_iterator a, b;
    REQUIRE_FALSE(a != b);
}

TEST_CASE( "skip_list/default iterator/const_iterator equality", "" )
{
    skip_list<int>::iterator a;
    skip_list<int>::const_iterator b;
    REQUIRE(a == b);
    REQUIRE(b == a);
}

TEST_CASE( "skip_list/default iterator/const_iterator inequality", "" )
{
    skip_list<int>::iterator a;
    skip_list<int>::const_iterator b;
    REQUIRE_FALSE(a != b);
    REQUIRE_FALSE(b != a);
}

TEST_CASE( "skip_list/empty list begin() is end()", "" )
{
    skip_list<int> list;
    
    REQUIRE(list.begin() == list.end());
    (list.cbegin() == list.cend());
    
    const skip_list<int> &clist(list);
    REQUIRE(clist.cbegin() == clist.cend());
}

//============================================================================
// Checking code compiles

TEST_CASE( "skip_list/iterator converts to const iterator", "" )
{
    skip_list<int> list;
    skip_list<int>::iterator i = list.begin();
    skip_list<int>::const_iterator ci = i;
    (void)ci;
    REQUIRE(true);
}

//============================================================================
// front

TEST_CASE( "skip_list/front/one item", "" )
{
    skip_list<int> list;
    const skip_list<int> &clist(list);

    list.insert(21);
    REQUIRE(list.front() == 21);
    REQUIRE(clist.front() == 21);
}

TEST_CASE( "skip_list/front/two items", "" )
{
    skip_list<int> list;
    const skip_list<int> &clist(list);
    
    list.insert(21);
    list.insert(7);
    REQUIRE(list.front() == 7);
    REQUIRE(clist.front() == 7);
}

//============================================================================
// back

TEST_CASE( "skip_list/back/one item", "" )
{
    skip_list<int> list;
    const skip_list<int> &clist(list);
    
    list.insert(21);
    REQUIRE(list.back() == 21);
    REQUIRE(clist.back() == 21);
}

TEST_CASE( "skip_list/back/two items", "" )
{
    skip_list<int> list;
    const skip_list<int> &clist(list);
    
    list.insert(21);
    list.insert(7);
    REQUIRE(list.back() == 21);
    REQUIRE(clist.back() == 21);
}

//============================================================================
// inserting one item

TEST_CASE( "skip_list/inserting one item is not empty()", "" )
{
    skip_list<int> list;
    list.insert(10);
    REQUIRE_FALSE(list.empty());
}

TEST_CASE( "skip_list/inserting one item begin() is not end()", "" )
{
    skip_list<int> list;
    list.insert(10);
    REQUIRE(list.begin() != list.end());
    REQUIRE(list.cbegin() != list.cend());
    REQUIRE_FALSE(list.begin() == list.end());
    REQUIRE_FALSE(list.cbegin() == list.cend());

    const skip_list<int> &clist(list);
    REQUIRE(clist.cbegin() != clist.cend());
    REQUIRE_FALSE(clist.cbegin() == clist.cend());
}

TEST_CASE( "skip_list/inserting one item returned from front()", "" )
{
    skip_list<int> list;
    list.insert(10);
    REQUIRE(list.front() == 10);
    
    const skip_list<int> &clist(list);
    REQUIRE(clist.front() == 10);
}

TEST_CASE( "skip_list/inserting one item returned from begin()", "" )
{
    skip_list<int> list;
    list.insert(10);
    REQUIRE(list.front() == 10);
    
    const skip_list<int> &clist(list);
    REQUIRE(clist.front() == 10);
}

TEST_CASE( "skip_list/inserting one item returns a count()", "" )
{
    skip_list<int> list;
    list.insert(10);
    REQUIRE(list.count(0) == 0);
    REQUIRE(list.count(1) == 0);
    REQUIRE(list.count(10) == 1);
    REQUIRE(list.count(11) == 0);
}

TEST_CASE( "skip_list/inserting one item returns correct contains()", "" )
{
    skip_list<int> list;
    list.insert(10);
    REQUIRE(list.contains(10));
    REQUIRE_FALSE(list.contains(1));
    REQUIRE_FALSE(list.contains(9));
    REQUIRE_FALSE(list.contains(11));
}

TEST_CASE( "skip_list/insert() returns iterator to inserted element", "" )
{
    skip_list<int> list;
    
    skip_list<int>::insert_by_value_result r = list.insert(10);
    REQUIRE(r.second);
    REQUIRE(*r.first == 10);
}

TEST_CASE( "skip_list/can't insert same item twice", "" )
{
    skip_list<int> list;
    
    list.insert(10);
    REQUIRE(list.size() == 1);

    skip_list<int>::insert_by_value_result r = list.insert(10);
    REQUIRE(!r.second);
    REQUIRE(r.first == list.end());
    REQUIRE(list.size() == 1);
    
    skip_list<int>::iterator i = list.begin();
    REQUIRE(*i++ == 10);
    REQUIRE(i == list.end());
}

//============================================================================
// erasing by value

TEST_CASE( "skip_list/erase/not contained item doesn't break things", "" )
{
    skip_list<int> list;
    list.insert(10);
    REQUIRE(list.erase(11) == 0);

    REQUIRE(list.count(10) == 1);
    REQUIRE(list.begin() != list.end());
}

TEST_CASE( "skip_list/erase/only item", "" )
{
    skip_list<int> list;
    list.insert(10);

    REQUIRE(list.erase(10) == 1);
    REQUIRE(list.count(10) == 0);
    REQUIRE(list.begin() == list.end());
    
    REQUIRE(list.erase(10) == 0);
}

TEST_CASE( "skip_list/erase/first item", "" )
{
    skip_list<int> list;
    list.insert(10);
    list.insert(20);
    
    REQUIRE(list.size() == 2);
    REQUIRE(list.erase(10) == 1);
    REQUIRE(list.size() == 1);
    REQUIRE(list.front() == 20);

    REQUIRE(list.count(10) == 0);
    REQUIRE(list.size() == 1);
}

TEST_CASE( "skip_list/erase/last item", "" )
{
    skip_list<int> list;
    list.insert(10);
    list.insert(20);
    
    REQUIRE(list.size() == 2);
    REQUIRE(list.erase(20) == 1);
    REQUIRE(list.size() == 1);
    REQUIRE(list.front() == 10);
    
    REQUIRE(list.count(20) == 0);
}

//============================================================================
// erasing by iterator

TEST_CASE( "skip_list/erase/iterator/empy list/end", "" )
{
    skip_list<int> list;
    // This will trigger an assertion, as it's invalid...
    //REQUIRE(list.erase(list.end()) == list.end());
}

TEST_CASE( "skip_list/erase/iterator/one item list/erase begin()", "" )
{
    skip_list<int> list;
    list.insert(45);
    list.erase(list.begin());
    REQUIRE(list.empty());
    REQUIRE(list.begin() == list.end());
}

TEST_CASE( "skip_list/erase/iterator/one item list/erase --end()", "" )
{
    skip_list<int> list;
    list.insert(45);
    list.erase(--list.end());
    REQUIRE(list.empty());
    REQUIRE(list.begin() == list.end());
}

TEST_CASE( "skip_list/erase/iterator/three item list/middle", "" )
{
    skip_list<int> list; list.insert(1); list.insert(2); list.insert(3);

    list.erase(++list.begin()); REQUIRE(list.size() == 2);
    
    skip_list<int>::iterator i = list.begin();
    REQUIRE(*i++ == 1); REQUIRE(*i++ == 3); REQUIRE(i == list.end());
}

TEST_CASE( "skip_list/erase/iterator/three item list/begin", "" )
{
    skip_list<int> list; list.insert(1); list.insert(2); list.insert(3);
    
    list.erase(list.begin()); REQUIRE(list.size() == 2);
    
    skip_list<int>::iterator i = list.begin();
    REQUIRE(*i++ == 2); REQUIRE(*i++ == 3); REQUIRE(i == list.end());
}

TEST_CASE( "skip_list/erase/iterator/three item list/end", "" )
{
    skip_list<int> list; list.insert(1); list.insert(2); list.insert(3);
    
    list.erase(--list.end()); REQUIRE(list.size() == 2);
    
    skip_list<int>::iterator i = list.begin();
    REQUIRE(*i++ == 1); REQUIRE(*i++ == 2); REQUIRE(i == list.end());
}

//============================================================================
// erasing by range

TEST_CASE( "vector/erase/iterator/three item list/start", "" )
{
    // This is a sanity check, to prove what's below is right
    std::vector<int> list; list.push_back(1); list.push_back(2); list.push_back(3); list.push_back(4);
    
    list.erase(list.begin(), ----list.end());
    REQUIRE(list.size() == 2);
    
    std::vector<int>::iterator i = list.begin();
    REQUIRE(*i++ == 3); REQUIRE(*i++ == 4); REQUIRE(i == list.end());
}

TEST_CASE( "skip_list/erase/iterator/three item list/start", "" )
{
    skip_list<int> list; list.insert(1); list.insert(2); list.insert(3); list.insert(4);
    
    list.erase(list.begin(), ----list.end());
    REQUIRE(list.size() == 2);
    
    skip_list<int>::iterator i = list.begin();
    REQUIRE(*i++ == 3); REQUIRE(*i++ == 4); REQUIRE(i == list.end());
}

TEST_CASE( "vector/erase/iterator/three item list/middle", "" )
{
    std::vector<int> list; list.push_back(1); list.push_back(2); list.push_back(3); list.push_back(4);
    
    list.erase(++list.begin(), ----list.end());
    REQUIRE(list.size() == 3);
    
    std::vector<int>::iterator i = list.begin();
    REQUIRE(*i++ == 1); REQUIRE(*i++ == 3); REQUIRE(*i++ == 4); REQUIRE(i == list.end());
}

TEST_CASE( "skip_list/erase/iterator/three item list/middle", "" )
{
    skip_list<int> list; list.insert(1); list.insert(2); list.insert(3); list.insert(4);
    
    list.erase(++list.begin(), ----list.end());
    REQUIRE(list.size() == 3);
    
    skip_list<int>::iterator i = list.begin();
    REQUIRE(*i++ == 1); REQUIRE(*i++ == 3); REQUIRE(*i++ == 4); REQUIRE(i == list.end());
}

//============================================================================
// size

TEST_CASE( "skip_list/empty list has size zero", "" )
{
    const skip_list<int> list;
    REQUIRE(list.size() == 0);
}

TEST_CASE( "skip_list/size of one item list", "" )
{
    skip_list<int> list;
    list.insert(69);
    REQUIRE(list.size() == 1);
}

TEST_CASE( "skip_list/size of list after erase", "" )
{
    skip_list<int> list;
    list.insert(69);
    list.erase(69);
    REQUIRE(list.size() == 0);
}

TEST_CASE( "skip_list/size of list after erasing item not in list", "" )
{
    skip_list<int> list;
    list.insert(69);
    list.erase(2);
    REQUIRE(list.size() == 1);
}

TEST_CASE( "skip_list/size of list after erase", "" )
{
    skip_list<int> list;
    list.insert(1);
    list.insert(2);
    list.insert(3);
    list.erase(2);
    REQUIRE(list.size() == 2);
    list.erase(1);
    REQUIRE(list.size() == 1);
    list.erase(1);
    REQUIRE(list.size() == 1);
    list.erase(3);
    REQUIRE(list.size() == 0);
}

//============================================================================
// insert with hint

TEST_CASE( "skip_list/insert-hint/bad", "" )
{
    skip_list<int> list;
    list.insert(10);
    list.insert(20);
    skip_list<int>::iterator hint = list.insert(30).first;
    REQUIRE(list.size() == 3);
    
    list.insert(hint, 15);
    REQUIRE(list.size() == 4);
}

TEST_CASE( "skip_list/insert-hint/good", "" )
{
    skip_list<int> list;
    list.insert(10);
    skip_list<int>::iterator hint = list.insert(20).first;
    list.insert(30);

    skip_list<int>::iterator i = list.insert(hint, 25);
    REQUIRE(list.size() == 4);
    REQUIRE(*i == 25);
}

//============================================================================
// random level selection

TEST_CASE( "skip_list/inserting one item returned from begin()", "" )
{/*
    skip_list_impl<int> list;
    std::vector<unsigned> levels(skip_list_impl<int>::max_levels, 0);
    for (unsigned n = 0; n < 10000; ++n)
    {
        unsigned random = list.random_level();
        REQUIRE(random < unsigned(skip_list_impl<int>::max_levels));
        levels[random]++;
    }
    //for (unsigned n = 0; n < skip_list<int>::max_levels; ++n)
    //    fprintf(stderr, "Level[%u]=%u\n", n, levels[n]);
    
    for (unsigned n = 0; n < skip_list_impl<int>::max_levels-1; ++n)
    {
        if (levels[n+1]) break;
        REQUIRE(levels[n] > levels[n+1]);
    }*/
}

//============================================================================
// iterating through a series of items

TEST_CASE( "skip_list/inserting an ordered series and iterating forwards", "" )
{
    skip_list<int> list;
    list.insert(10);
    list.insert(20);
    list.insert(30);
    list.insert(40);
    
    skip_list<int>::iterator i = list.begin();

    REQUIRE(i != list.end()); REQUIRE(*i == 10); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 20); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 30); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 40); ++i;
    REQUIRE(i == list.end());
}

TEST_CASE( "skip_list/inserting an unordered series and iterating forwards", "" )
{
    skip_list<int> list;
    list.insert(30);
    list.insert(10);
    list.insert(40);
    list.insert(20);
    list.insert(0);
    
    skip_list<int>::iterator i = list.begin();
    
    REQUIRE(i != list.end()); REQUIRE(*i == 0); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 10); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 20); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 30); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 40); ++i;
    REQUIRE(i == list.end());
}

TEST_CASE( "skip_list/inserting a series and iterating forwards and backwards", "" )
{
    skip_list<int> list;
    list.insert(30);
    list.insert(10);
    list.insert(40);
    list.insert(20);
    list.insert(0);
    
    skip_list<int>::iterator i = list.begin();
    
    REQUIRE(i != list.end()); REQUIRE(*i == 0); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 10); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 20); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 30); --i;
    REQUIRE(i != list.end()); REQUIRE(*i == 20); --i;
    REQUIRE(i != list.end()); REQUIRE(*i == 10); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 20); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 30); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 40); ++i;
    REQUIRE(i == list.end());
}

TEST_CASE( "skip_list/inserting an unordered series and iterating backwards", "" )
{
    skip_list<int> list;
    list.insert(30);
    list.insert(10);
    list.insert(40);
    list.insert(20);
    list.insert(0);
    
    skip_list<int>::iterator i = list.end();
    
    --i; REQUIRE(i != list.end());

    REQUIRE(i != list.begin()); REQUIRE(*i == 40); --i;
    REQUIRE(i != list.begin()); REQUIRE(*i == 30); --i;
    REQUIRE(i != list.begin()); REQUIRE(*i == 20); --i;
    REQUIRE(i != list.begin()); REQUIRE(*i == 10); --i;
    REQUIRE(i == list.begin()); REQUIRE(*i == 0);
}

//============================================================================
// assign

int assign_source_data[]    = { 45, 34, 67, 12 };
int *assign_source_data_end = assign_source_data + 4;

TEST_CASE( "skip_list/assign/empty list", "" )
{
    skip_list<int> list;
    list.assign(assign_source_data, assign_source_data_end);
    REQUIRE(list.size() == 4);

    skip_list<int>::iterator i = list.begin();
    REQUIRE(*i == 12); ++i; REQUIRE(i != list.end());
    REQUIRE(*i == 34); ++i; REQUIRE(i != list.end());
    REQUIRE(*i == 45); ++i; REQUIRE(i != list.end());
    REQUIRE(*i == 67); ++i; REQUIRE(i == list.end());
}

TEST_CASE( "skip_list/assign/populated list", "" )
{
    skip_list<int> list;
    list.insert(1); list.insert(2); list.insert(300);

    list.assign(assign_source_data, assign_source_data_end);
    REQUIRE(list.size() == 4);
    
    skip_list<int>::iterator i = list.begin();
    REQUIRE(*i == 12); ++i; REQUIRE(i != list.end());
    REQUIRE(*i == 34); ++i; REQUIRE(i != list.end());
    REQUIRE(*i == 45); ++i; REQUIRE(i != list.end());
    REQUIRE(*i == 67); ++i; REQUIRE(i == list.end());
}

//============================================================================
// operator=

TEST_CASE( "skip_list/operator=/assignment copies data", "" )
{
    skip_list<int> source;
    source.assign(assign_source_data, assign_source_data_end);
    
    skip_list<int> assignee;
    assignee = source;

    skip_list<int>::iterator i1 = source.begin();
    skip_list<int>::iterator i2 = assignee.begin();
    REQUIRE(*i1 == 12); REQUIRE(*i2 == 12); ++i1; ++i2; REQUIRE(i1 != source.end());
    REQUIRE(*i1 == 34); REQUIRE(*i2 == 34); ++i1; ++i2; REQUIRE(i1 != source.end());
    REQUIRE(*i1 == 45); REQUIRE(*i2 == 45); ++i1; ++i2; REQUIRE(i1 != source.end());
    REQUIRE(*i1 == 67); REQUIRE(*i2 == 67); ++i1; ++i2; REQUIRE(i1 == source.end());
}

TEST_CASE( "skip_list/operator=/assignment returns assignee", "" )
{
    skip_list<int> source;
    source.assign(assign_source_data, assign_source_data_end);
    
    skip_list<int> assignee;
    skip_list<int> &returned = (assignee = source);
    
    REQUIRE(&returned == &assignee);
}

TEST_CASE( "skip_list/operator=/assignment still maintains seprate lists", "" )
{
    skip_list<int> source;
    source.assign(assign_source_data, assign_source_data_end);
    
    skip_list<int> assignee;
    assignee = source;

    source.insert(0);
    
    REQUIRE(source.size() == 5);
    REQUIRE(assignee.size() == 4);
}

//============================================================================
// copy ctor

TEST_CASE( "skip_list/copy ctor/copy copies data", "" )
{
    skip_list<int> source;
    source.assign(assign_source_data, assign_source_data_end);
    
    skip_list<int> assignee(source);
    
    skip_list<int>::iterator i1 = source.begin();
    skip_list<int>::iterator i2 = assignee.begin();
    REQUIRE(*i1 == 12); REQUIRE(*i2 == 12); ++i1; ++i2; REQUIRE(i1 != source.end());
    REQUIRE(*i1 == 34); REQUIRE(*i2 == 34); ++i1; ++i2; REQUIRE(i1 != source.end());
    REQUIRE(*i1 == 45); REQUIRE(*i2 == 45); ++i1; ++i2; REQUIRE(i1 != source.end());
    REQUIRE(*i1 == 67); REQUIRE(*i2 == 67); ++i1; ++i2; REQUIRE(i1 == source.end());
}

TEST_CASE( "skip_list/ocopy ctor/copy still maintains seprate lists", "" )
{
    skip_list<int> source;
    source.assign(assign_source_data, assign_source_data_end);
    
    skip_list<int> assignee(source);
    
    source.insert(0);
    
    REQUIRE(source.size() == 5);
    REQUIRE(assignee.size() == 4);
}

//============================================================================
// copy ctor with allocator

TEST_CASE( "skip_list/copy ctor with allocator/copy copies data", "" )
{
    skip_list<int> source;
    source.assign(assign_source_data, assign_source_data_end);

    std::allocator<int> alloc;
    skip_list<int> assignee(source, alloc);
    
    skip_list<int>::iterator i1 = source.begin();
    skip_list<int>::iterator i2 = assignee.begin();
    REQUIRE(*i1 == 12); REQUIRE(*i2 == 12); ++i1; ++i2; REQUIRE(i1 != source.end());
    REQUIRE(*i1 == 34); REQUIRE(*i2 == 34); ++i1; ++i2; REQUIRE(i1 != source.end());
    REQUIRE(*i1 == 45); REQUIRE(*i2 == 45); ++i1; ++i2; REQUIRE(i1 != source.end());
    REQUIRE(*i1 == 67); REQUIRE(*i2 == 67); ++i1; ++i2; REQUIRE(i1 == source.end());
}

TEST_CASE( "skip_list/ocopy ctor with allocator/copy still maintains seprate lists", "" )
{
    skip_list<int> source;
    source.assign(assign_source_data, assign_source_data_end);
    
    std::allocator<int> alloc;
    skip_list<int> assignee(source, alloc);
    
    source.insert(0);
    
    REQUIRE(source.size() == 5);
    REQUIRE(assignee.size() == 4);
}

//============================================================================
// swap

TEST_CASE( "skip_list/swap", "" )
{
    skip_list<int> l1, l2;
    l1.insert(1);
    l1.insert(2);
    l1.insert(3);
    
    l1.swap(l2);
    REQUIRE(l1.size() == 0);
    REQUIRE(l2.size() == 3);
    
    l1.swap(l2);
    REQUIRE(l1.size() == 3);
    REQUIRE(l2.size() == 0);
    
    l2.swap(l1);
    REQUIRE(l1.size() == 0);
    REQUIRE(l2.size() == 3);
}

//============================================================================
// insert(iter,iter)

TEST_CASE( "skip_list/insert(iter,iter)/empty list", "" )
{
    skip_list<int> list;
    list.insert(assign_source_data, assign_source_data_end);
    
    REQUIRE(list.size() == 4);
    skip_list<int>::iterator i = list.begin();
    REQUIRE(*i == 12); ++i; REQUIRE(i != list.end());
    REQUIRE(*i == 34); ++i; REQUIRE(i != list.end());
    REQUIRE(*i == 45); ++i; REQUIRE(i != list.end());
    REQUIRE(*i == 67); ++i; REQUIRE(i == list.end());
}

TEST_CASE( "skip_list/insert(iter,iter)/returns iterator following last inserted element", "" )
{
    skip_list<int> list;
    skip_list<int>::iterator i = list.insert(assign_source_data, assign_source_data_end);
    
    REQUIRE(i != list.end()); // magic because the list of inserted items is out of order!
    REQUIRE(*i == 34);
}

TEST_CASE( "skip_list/insert(iter,iter)/empty list, empty range", "" )
{
    skip_list<int> list;
    skip_list<int>::iterator i = list.insert(assign_source_data, assign_source_data);
    
    REQUIRE(list.size() == 0);
    REQUIRE(i == list.end());
}

TEST_CASE( "skip_list/insert(iter,iter)/populated list, empty range", "" )
{
    skip_list<int> list;
    list.insert(0);
    list.insert(1000);
    skip_list<int>::iterator i = list.insert(assign_source_data, assign_source_data);
    
    REQUIRE(list.size() == 2);
    REQUIRE(i == list.end());
}

TEST_CASE( "skip_list/insert(iter,iter)/populated list, insert range", "" )
{
    skip_list<int> list;
    list.insert(0);
    list.insert(1000);
    skip_list<int>::iterator i = list.insert(assign_source_data, assign_source_data_end);
    
    REQUIRE(list.size() == 6);
    REQUIRE(i != list.end());
    REQUIRE(*i == 34); // as above - because source list is out-of-order
}

//============================================================================
// find

TEST_CASE( "skip_list/find/empty list", "" )
{
    skip_list<int> list;
    const skip_list<int> &clist = list;

    REQUIRE(list.find(12)  == list.end());
    REQUIRE(clist.find(12) == list.end());
    REQUIRE(clist.find(12) == clist.end());
}

TEST_CASE( "skip_list/find/in a populated list", "" )
{
    skip_list<int> list;
    const skip_list<int> &clist = list;

    list.insert(30);
    list.insert(10);
    list.insert(40);
    list.insert(20);
    list.insert(0);
    
    REQUIRE(list.find(12) == list.end());
    REQUIRE(clist.find(12) == list.end());
    REQUIRE(clist.find(12) == clist.end());

    REQUIRE(list.find(10) != list.end());
    REQUIRE(*list.find(10) == 10);
    REQUIRE(*list.find(20) == 20);
    REQUIRE(*list.find(30) == 30);
    REQUIRE(*list.find(40) == 40);
    REQUIRE(*list.find(0) == 0);
    
    REQUIRE(clist.find(10) != clist.end());
    REQUIRE(*clist.find(10) == 10);
    REQUIRE(*clist.find(20) == 20);
    REQUIRE(*clist.find(30) == 30);
    REQUIRE(*clist.find(40) == 40);
    REQUIRE(*clist.find(0) == 0);
}

//============================================================================
// clear

TEST_CASE( "skip_list/clear/empty list", "" )
{
    skip_list<int> list;
    list.clear();
    REQUIRE(true); // didn't crash :-)
    REQUIRE(list.size() == 0);
}

TEST_CASE( "skip_list/clear/one item list", "" )
{
    skip_list<int> list;
    list.insert(1);
    list.clear();
    REQUIRE(list.size() == 0);
    REQUIRE(list.empty());
    REQUIRE(list.begin() == list.end());
}

TEST_CASE( "skip_list/clear/two item list", "" )
{
    skip_list<int> list;
    for (int n = 0; n < 2; ++n) list.insert(n);
    
    list.clear();
    REQUIRE(list.size() == 0);
    REQUIRE(list.empty());
    REQUIRE(list.begin().get_parent() == list.end().get_parent());
    REQUIRE(list.begin().get_node() == list.end().get_node());
    REQUIRE(list.begin() == list.end());
}

TEST_CASE( "skip_list/clear/several item list", "" )
{
    skip_list<int> list;
    for (int n = 0; n < 10; ++n) list.insert(n);

    list.clear();
    REQUIRE(list.size() == 0);
    REQUIRE(list.empty());
    REQUIRE(list.begin() == list.end());
}

//============================================================================
// list comparison

TEST_CASE( "skip_list/comparison/two empty lists", "" )
{
    skip_list<int> l1, l2;
    
    REQUIRE(l1 == l2);      REQUIRE(l2 == l1);
    REQUIRE_FALSE(l1 != l2); REQUIRE_FALSE(l2 != l1);
    
    REQUIRE_FALSE(l1 < l2); REQUIRE_FALSE(l2 < l1);
    REQUIRE_FALSE(l1 > l2); REQUIRE_FALSE(l2 > l1);

    REQUIRE(l1 <= l2); REQUIRE(l2 <= l1);
    REQUIRE(l1 >= l2); REQUIRE(l2 >= l1);
}

TEST_CASE( "skip_list/comparison/one empty list", "" )
{
    skip_list<int> l1, l2;
    l1.insert(1);
    
    REQUIRE(l1 != l2);       REQUIRE(l2 != l1);
    REQUIRE_FALSE(l1 == l2); REQUIRE_FALSE(l2 == l1);
    
    REQUIRE_FALSE(l1 < l2); REQUIRE(l2 < l1);
    REQUIRE(l1 > l2);       REQUIRE_FALSE(l2 > l1);
    
    REQUIRE_FALSE(l1 <= l2); REQUIRE(l2 <= l1);
    REQUIRE(l1 >= l2);       REQUIRE_FALSE(l2 >= l1);
}

//============================================================================
// lifetime of objects in the container

struct Counter
{
    static int count;

    Counter() : value(0) { ++count; } // TODO: do not require this

    Counter(int i) : value(i) { ++count; }
    ~Counter() { --count; }
    Counter(const Counter &other) : value(other.value) { ++count; }
    Counter &operator=(const Counter &other) { value = other.value; return *this; }
    
    int value;
    
    bool operator<(const Counter &other) const { return value < other.value; }
    
    int get_int() const { return value;}

    // TODO: this shouldn't be a requirement
    bool operator==(const Counter &rhs) const { return value == rhs.value; }
    bool operator<=(const Counter &rhs) const { return value <= rhs.value; }
};

int Counter::count = 0;

TEST_CASE( "Counter/sanity test", "" )
{
    REQUIRE(Counter::count == 0);

    { Counter(1); REQUIRE(Counter::count == 0); }
    
    { Counter c; REQUIRE(Counter::count == 1); }
    REQUIRE(Counter::count == 0);
    
    { Counter c(1); REQUIRE(Counter::count == 1); }
    REQUIRE(Counter::count == 0);

    { Counter c1(1); Counter c2(2); REQUIRE(Counter::count == 2); }
    REQUIRE(Counter::count == 0);

    { Counter c1(1); Counter c2(c1); REQUIRE(Counter::count == 2); }
    REQUIRE(Counter::count == 0);
    
    { Counter c1(1); Counter c2(2); c2 = c1; REQUIRE(Counter::count == 2); }
    REQUIRE(Counter::count == 0);
}

TEST_CASE( "skip_list/object lifetime", "" )
{
    REQUIRE(Counter::count == 0);
    {
        skip_list<Counter> list;
        REQUIRE(Counter::count == 0);
        
        list.insert(Counter(1));
        REQUIRE(Counter::count == 1);
        
        REQUIRE(list.size() == 1);
        list.clear();
        REQUIRE(Counter::count == 0);
        REQUIRE(list.size() == 0);
    }
    REQUIRE(Counter::count == 0);
}

TEST_CASE( "skip_list/remove/two item list object lifetime", "" )
{
    skip_list<Counter> list;

    list.insert(1);
    list.insert(2);
    
    REQUIRE(list.size() == 2);      REQUIRE(Counter::count == 2);
    REQUIRE(list.erase(1) == 1);    REQUIRE(Counter::count == 1);
    REQUIRE(list.erase(1) == 0);    REQUIRE(Counter::count == 1);
    
    {
        skip_list<Counter>::iterator i = list.begin();
        REQUIRE(i != list.end());
        REQUIRE(*i == 2);
        ++i;
        REQUIRE(i == list.end());
    }

    REQUIRE(list.erase(2) == 1);    REQUIRE(Counter::count == 0);

    REQUIRE(list.begin() == list.end());
}

TEST_CASE( "skip_list/clear/object lifetime", "" )
{
    Counter::count = 0;
    {
        skip_list<Counter> list;

        for (int n = 0; n < 10; ++n) list.insert(n);
        list.clear();
        REQUIRE(Counter::count == 0);
    }
    REQUIRE(Counter::count == 0);
}

TEST_CASE( "skip_list/erase/iterator/object lifetime", "" )
{
    Counter::count = 0;
    {
        skip_list<Counter> list; list.insert(1); list.insert(2); list.insert(3); list.insert(4);
        REQUIRE(Counter::count == 4);
        
        list.erase(++list.begin(), --list.end());
        REQUIRE(list.size() == 2);
        REQUIRE(Counter::count == 2);

        skip_list<Counter>::iterator i = list.begin();
        REQUIRE(*i++ == 1); REQUIRE(*i++ == 4); REQUIRE(i == list.end());
    }
    REQUIRE(Counter::count == 0);
}
