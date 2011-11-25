//============================================================================
// tests.cpp
// Copyright (c) 2011 Pete Goodliffe. All rights reserved
//============================================================================

#include "skip_list.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

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

struct TestingAllocator : Struct
{
    TestingAllocator() : Struct() {}
    TestingAllocator(int i, float f) : Struct(i,f) {}

    template <typename T>
    struct rebind { typedef TestingAllocator other; };
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
    REQUIRE((skip_list<int,std::less<int>,TestingAllocator>().get_allocator()) == TestingAllocator());
}

TEST_CASE( "skip_list/construction with allocator returns copy of that allocator", "" )
{
    REQUIRE((skip_list<int,std::less<int>,TestingAllocator>(TestingAllocator(10,4)).get_allocator()) == TestingAllocator(10,4));
}

TEST_CASE( "skip_list/constructed list returns empty()", "" )
{
    skip_list<int> list;
    REQUIRE(list.empty());
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
    list.insert(list.cbegin(), 10);
    REQUIRE_FALSE(list.empty());
}

TEST_CASE( "skip_list/inserting one item returned from front()", "" )
{
    skip_list<int> list;
    list.insert(list.cbegin(), 10);
    REQUIRE(list.front() == 10);
    
    const skip_list<int> &clist(list);
    REQUIRE(clist.front() == 10);
}
