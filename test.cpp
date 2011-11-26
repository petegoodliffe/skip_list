//============================================================================
// tests.cpp
// Copyright (c) 2011 Pete Goodliffe. All rights reserved
//============================================================================

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

template <typename T = int>
struct TestingAllocator : Struct
{
    TestingAllocator() : Struct() {}
    TestingAllocator(int i, float f) : Struct(i,f) {}

    template <typename OTHER>
    struct rebind { typedef TestingAllocator<OTHER> other; };
    
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T*        pointer;
    typedef const T*  const_pointer;
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

//============================================================================
// erasing

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
// random level selection

TEST_CASE( "skip_list/inserting one item returned from begin()", "" )
{
    skip_list_impl<int> list;
    std::vector<unsigned> levels(skip_list_impl<int>::max_levels, 0);
    for (unsigned n = 0; n < 10000; ++n)
    {
        unsigned random = list.random_level();
        REQUIRE(random < skip_list_impl<int>::max_levels);
        levels[random]++;
    }
    //for (unsigned n = 0; n < skip_list<int>::max_levels; ++n)
    //    fprintf(stderr, "Level[%u]=%u\n", n, levels[n]);
    
    for (unsigned n = 0; n < skip_list_impl<int>::max_levels-1; ++n)
    {
        if (levels[n+1]) break;
        REQUIRE(levels[n] > levels[n+1]);
    }
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
    //list.dump();
    
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
