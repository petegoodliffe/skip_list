//============================================================================
// tests.cpp
// Copyright (c) 2011 Pete Goodliffe. All rights reserved
//============================================================================

// MSVS complains about using std::equal unless we define thiis.
// It's a handy warning in the light of all the other evil you can
// wreak with STL iterators. It's like a post to tie your jelly to.
#define _SCL_SECURE_NO_WARNINGS

#include "skip_list.h"

#define CATCH_CONFIG_NO_STREAM_REDIRECTION
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "test_types.h"

using goodliffe::skip_list;
using goodliffe::detail::sl_impl;

TEST_CASE( "skip_list/smoketest", "" )
{
    //REQUIRE(false);
}

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
    REQUIRE((skip_list<int,std::less<int>,MockAllocator<int> >().get_allocator()) == MockAllocator<int>());
}

TEST_CASE( "skip_list/construction with allocator returns copy of that allocator", "" )
{
    REQUIRE((skip_list<int,std::less<int>,MockAllocator<int> >(MockAllocator<int>(10,4)).get_allocator()) == MockAllocator<int>(10,4));
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

TEST_CASE( "skip_list/impl/double insert middle value", "" )
{
    skip_list<int> list;
    
    list.insert(10);
    list.insert(30);
    list.insert(20);
    REQUIRE(list.size() == 3);

    skip_list<int>::insert_by_value_result r = list.insert(20);
    REQUIRE(!r.second);
    REQUIRE(list.size() == 3);
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
// iteration

TEST_CASE( "skip_list/iterator/empty list", "" )
{
    skip_list<int> list;
    
    REQUIRE(list.begin() == list.end());
    REQUIRE(list.cbegin() == list.cend());
    REQUIRE(list.rbegin() == list.rend());
    REQUIRE(list.crbegin() == list.crend());
}

TEST_CASE( "skip_list/iterator/non-const converts to const", "" )
{
    skip_list<int> list;
    
    REQUIRE(list.begin() == list.cbegin());
    REQUIRE(list.cbegin() == list.begin());
    REQUIRE(list.rbegin() == list.crbegin());
    REQUIRE(list.crbegin() == list.rbegin());
}

TEST_CASE( "skip_list/iterator/one item/forwards", "" )
{
    skip_list<int> list;
    list.insert(1);
    
    REQUIRE(list.begin() != list.end());
    REQUIRE(list.cbegin() != list.cend());
    REQUIRE(list.begin() != list.cend());
    
    skip_list<int>::iterator i = list.begin();
    REQUIRE(*i++ == 1);
    REQUIRE(i == list.end());
}

TEST_CASE( "skip_list/iterator/one item/reverse", "" )
{
    skip_list<int> list;
    list.insert(1);
    
    REQUIRE(list.rbegin() != list.rend());
    REQUIRE(list.crbegin() != list.crend());
    REQUIRE(list.rbegin() != list.crend());
    
    skip_list<int>::reverse_iterator i = list.rbegin();
    REQUIRE(*i++ == 1);
    REQUIRE(i == list.rend());
}

TEST_CASE( "skip_list/iterator/three item/forward", "" )
{
    skip_list<int> list;
    list.insert(1); list.insert(2); list.insert(3);

    skip_list<int>::iterator i = list.begin();
    REQUIRE(*i++ == 1);
    REQUIRE(*i++ == 2);
    REQUIRE(*i++ == 3);
    REQUIRE(i == list.end());
}

TEST_CASE( "skip_list/iterator/three item/forward/preincrement", "" )
{
    skip_list<int> list;
    list.insert(1); list.insert(2); list.insert(3);
    
    skip_list<int>::iterator i = list.begin();
    REQUIRE(*++i == 2);
    REQUIRE(*++i == 3);
    ++i; // can't dererence end() :-)
    REQUIRE(i == list.end());
}

TEST_CASE( "skip_list/iterator/three item/reverse", "" )
{
    skip_list<int> list;
    list.insert(1); list.insert(2); list.insert(3);
    
    skip_list<int>::reverse_iterator i = list.rbegin();
    REQUIRE(*i++ == 3);
    REQUIRE(*i++ == 2);
    REQUIRE(*i++ == 1);
    REQUIRE(i == list.rend());
}

TEST_CASE( "skip_list/iterator/three item/forward/reverse/preincrement", "" )
{
    skip_list<int> list;
    list.insert(1); list.insert(2); list.insert(3);
    
    skip_list<int>::reverse_iterator i = list.rbegin();
    REQUIRE(*++i == 2);
    REQUIRE(*++i == 1);
    ++i; // can't dererence rend() :-)
    REQUIRE(i == list.rend());
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
    
    skip_list<int>::reverse_iterator ri = list.rbegin();
    REQUIRE(*ri++ == 4); REQUIRE(*ri++ == 3); REQUIRE(ri == list.rend());
}

TEST_CASE( "vector/erase/iterator/three item list/middle", "" )
{
    // sanity check
    std::vector<int> list; list.push_back(1); list.push_back(2); list.push_back(3); list.push_back(4);
    
    list.erase(++list.begin(), ----list.end());
    REQUIRE(list.size() == 3);
    
    std::vector<int>::iterator i = list.begin();
    REQUIRE(*i++ == 1); REQUIRE(*i++ == 3); REQUIRE(*i++ == 4); REQUIRE(i == list.end());
    
    std::vector<int>::reverse_iterator ri = list.rbegin();
    REQUIRE(*ri++ == 4); REQUIRE(*ri++ == 3); REQUIRE(*ri++ == 1); REQUIRE(ri == list.rend());
}

TEST_CASE( "skip_list/erase/iterator/three item list/middle", "" )
{
    skip_list<int> list; list.insert(1); list.insert(2); list.insert(3); list.insert(4);
    
    list.erase(++list.begin(), ----list.end());
    REQUIRE(list.size() == 3);
    
    skip_list<int>::iterator i = list.begin();
    REQUIRE(*i++ == 1); REQUIRE(*i++ == 3); REQUIRE(*i++ == 4); REQUIRE(i == list.end());
    
    skip_list<int>::reverse_iterator ri = list.rbegin();
    REQUIRE(*ri++ == 4); REQUIRE(*ri++ == 3); REQUIRE(*ri++ == 1); REQUIRE(ri == list.rend());
}

TEST_CASE( "skip_list/erase/iterator/three item list/end", "" )
{
    skip_list<int> list; list.insert(1); list.insert(2); list.insert(3); list.insert(4);
    
    list.erase(++++list.begin(), list.end());
    REQUIRE(list.size() == 2);

    skip_list<int>::iterator i = list.begin();
    REQUIRE(*i++ == 1); REQUIRE(*i++ == 2); REQUIRE(i == list.end());
    
    skip_list<int>::reverse_iterator ri = list.rbegin();
    REQUIRE(*ri++ == 2); REQUIRE(*ri++ == 1); REQUIRE(ri == list.rend());
}

TEST_CASE( "skip_list/erase/iterator/three item list/whole list", "" )
{
    skip_list<int> list; list.insert(1); list.insert(2); list.insert(3); list.insert(4);
    
    list.erase(list.begin(), list.end());
    REQUIRE(list.size() == 0);
    
    skip_list<int>::iterator i = list.begin();
    REQUIRE(i == list.end());
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

TEST_CASE( "skip_list_level_generator/random level algorithm", "" )
{
    using goodliffe::detail::skip_list_level_generator;

    skip_list_level_generator<32> list;
    std::vector<unsigned> levels(skip_list_level_generator<32>::num_levels, 0);
    for (unsigned n = 0; n < 10000; ++n)
    {
        unsigned random = list.new_level();
        REQUIRE(random < unsigned(skip_list_level_generator<32>::num_levels));
        levels[random]++;
    }
    //for (unsigned n = 0; n < skip_list_level_generator<32>::num_levels; ++n)
    //    fprintf(stderr, "Level[%u]=%u\n", n, levels[n]);
    
    for (unsigned n = 0; n < skip_list_level_generator<32>::num_levels-1; ++n)
    {
        if (levels[n+1]) break;
        REQUIRE(levels[n] > levels[n+1]);
    }
}

TEST_CASE( "skip_list_level_generator/compilation errors", "" )
{
    goodliffe::detail::skip_list_level_generator<33> list;
    (void)list;

    // This should not run if uncommented!
    //list.new_level();
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

TEST_CASE( "skip_list/swap/iterators", "" )
{
    skip_list<int> l1, l2;
    l1.insert(1);
    l1.insert(2);
    l1.insert(3);

    l2.insert(10);
    l2.insert(20);
    l2.insert(30);
    
    skip_list<int>::iterator i1 = l1.begin(), i2 = l2.begin();
    
    swap(l1, l2);
    REQUIRE(*i1 == 1); ++i1;
    REQUIRE(*i1 == 2); ++i1;
    REQUIRE(*i1 == 3); ++i1;
    REQUIRE(*i2 == 10); ++ i2;
    REQUIRE(*i2 == 20); ++ i2;
    REQUIRE(*i2 == 30); ++ i2;

    i1 = l1.begin();
    i2 = l2.begin();
    
    std::swap(l1, l2);
    REQUIRE(*i1 == 10); ++i1;
    REQUIRE(*i1 == 20); ++i1;
    REQUIRE(*i1 == 30); ++i1;
    REQUIRE(*i2 == 1); ++ i2;
    REQUIRE(*i2 == 2); ++ i2;
    REQUIRE(*i2 == 3); ++ i2;
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

TEST_CASE( "skip_list/insert(iter,iter)/empty list, empty range", "" )
{
    skip_list<int> list;
    list.insert(assign_source_data, assign_source_data);
    
    REQUIRE(list.size() == 0);
}

TEST_CASE( "skip_list/insert(iter,iter)/populated list, empty range", "" )
{
    skip_list<int> list;
    list.insert(0);
    list.insert(1000);
    list.insert(assign_source_data, assign_source_data);
    
    REQUIRE(list.size() == 2);
}

TEST_CASE( "skip_list/insert(iter,iter)/populated list, insert range", "" )
{
    skip_list<int> list;
    list.insert(0);
    list.insert(1000);
    list.insert(assign_source_data, assign_source_data_end);
    REQUIRE(list.size() == 6);
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
#ifdef SKIP_LIST_IMPL_DIAGNOSTICS    
    REQUIRE(list.begin().get_impl() == list.end().get_impl());
#endif
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

//============================================================================
// the mother of all comparison tests

TEST_CASE( "skip_list/comparison with vector", "" )
{
    std::set<int> s;
    skip_list<int> l;
    
    for (unsigned repeats = 0; repeats < 5; ++repeats)
    {
        for (unsigned n = 0; n < 400; ++n)
        {
            int value = rand();
            s.insert(value);
            l.insert(value);
        }
        
        REQUIRE(CheckEquality(s, l));
        REQUIRE(CheckForwardIteration(l));
        REQUIRE(CheckBackwardIteration(l));
        
        unsigned erase_from   = unsigned(rand()) % unsigned(s.size()/3);
        unsigned erase_length = unsigned(rand()) % unsigned(s.size()/3);
        
        REQUIRE((erase_from+erase_length) <= s.size());
        
        std::set<int>::iterator  si_from = s.begin();
        skip_list<int>::iterator li_from = l.begin();
        std::advance(si_from, erase_from);
        std::advance(li_from, erase_from);
        
        std::set<int>::iterator  si_to = si_from;
        skip_list<int>::iterator li_to = li_from;
        std::advance(si_to, erase_length);
        std::advance(li_to, erase_length);
        
        s.erase(si_from, si_to);
        l.erase(li_from, li_to);
        
        REQUIRE(CheckEquality(s, l));
        REQUIRE(CheckForwardIteration(l));
        REQUIRE(CheckBackwardIteration(l));
    }
}

//============================================================================
// checking constructs based on Compare functor

TEST_CASE( "detail/equivalent", "" )
{
    using namespace goodliffe::detail;
    std::less<int> less_than;
    REQUIRE(equivalent(10, 10, less_than));
    REQUIRE_FALSE(equivalent(10, 11,less_than));
    REQUIRE_FALSE(equivalent(11, 10, less_than));
}

TEST_CASE( "detail/less_than_or_equal", "" )
{
    using namespace goodliffe::detail;
    std::less<int> less_than;
    REQUIRE(less_or_equal(10, 10, less_than));
    REQUIRE(less_or_equal(10, 11, less_than));
    REQUIRE_FALSE(less_or_equal(10, 9, less_than));
}

//============================================================================
// checking requirements on type

struct Value
{
    int value;
};

struct LessValue
{
    bool operator()(const Value &lhs, const Value &rhs) const
        { return lhs.value < rhs.value; }
};

template <class STREAM>
STREAM &operator<<(STREAM &s, const Value &v)
    { s << v.value; return s; }

/*
bool operator<(const Value &lhs, const Value &rhs);
bool operator<(const Value &lhs, const Value &rhs)
{
    return lhs.value < rhs.value;
}
*/

TEST_CASE( "skip_list/Value sanity test", "" )
{
    const Value a = {10};

    Value b = a; // assignment works
    
    b.value = 20;
    
    REQUIRE(LessValue()(a, b));
    REQUIRE_FALSE(LessValue()(a, a));
    REQUIRE_FALSE(LessValue()(b, a));

    // These will not compile
    //REQUIRE(a < b);
    //REQUIRE_FALSE(b < a);
    //REQUIRE(a <= b);
    //REQUIRE(a > b);
    //REQUIRE(a >= b);
}

TEST_CASE( "skip_list/type requirements (only <)", "" )
{
    const Value a = {10};
    const Value b = {0};
    const Value values[] = {{10},{20},{30}};

    skip_list<Value, LessValue> list;
    
    list.insert(a);
    list.erase(a);

    skip_list<Value, LessValue> list2(list);
    list2.clear();
    
    list.assign(values, values+3);
    REQUIRE(list.count(a) == 1);
    REQUIRE(list.count(b) == 0);

    REQUIRE(list.contains(a));
    REQUIRE_FALSE(list.contains(b));

    REQUIRE(list.find(a) != list.end());
    REQUIRE(list.find(b) == list.end());

    list2.insert(values, values+3);
    
    std::swap(list, list2);
    
    list2.erase(++list2.begin(), list2.end());
    REQUIRE(list2.size() == 1);
    REQUIRE_FALSE(list2.empty());
}

//============================================================================
// things that should not be compilable

TEST_CASE( "skip_list/insert(iter,iter)/populated list, insert range", "" )
{
    skip_list<int> list;
    const skip_list<int> &clist = list;
    list.insert(0);
    
    // Uncomment these lines to check they don't compile!

    // Iterators should not be assignable-thoughable
    //*list.begin() = 1; REQUIRE(list.front() == 1);
    //*clist.begin() = 1; REQUIRE(list.front() == 1);
    //*list.cbegin() = 1; REQUIRE(list.front() == 1);
    
    (void) clist;
}
