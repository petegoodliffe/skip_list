//============================================================================
// test_multi_skip_list.cpp
// Copyright (c) 2011 Pete Goodliffe. All rights reserved
//============================================================================

// MSVS complains about using std::equal unless we define thiis.
// It's a handy warning in the light of all the other evil you can
// wreak with STL iterators. It's like a post to tie your jelly to.
#define _SCL_SECURE_NO_WARNINGS

#include "skip_list.h"

#define CATCH_CONFIG_NO_STREAM_REDIRECTION 1
#include "catch.hpp"
#include "test_types.h"

#include <set>

using goodliffe::multi_skip_list;

//==============================================================================

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning (disable : 4068 ) /* disable unknown pragma warnings */
#endif

//==============================================================================

TEST_CASE( "multi_skip_list/smoketest", "" )
{
    //REQUIRE(false);
}

TEST_CASE( "multi_skip_list/is constructable", "" )
{
    multi_skip_list<int>    sl_int;
    multi_skip_list<float>  sl_float;
    multi_skip_list<Struct> sl_struct;
    
    REQUIRE(true);
}

TEST_CASE( "multi_skip_list/can call basic methods from skip_list", "" )
{
    const multi_skip_list<int> list;
    REQUIRE(list.empty());
    REQUIRE(list.size() == 0);
    REQUIRE(list.get_allocator() == std::allocator<int>());
    REQUIRE(list.find(10) == list.end());
    REQUIRE(list.count(0) == 0);
    REQUIRE(!list.contains(20));
    REQUIRE(list.max_size() > 67890);
    REQUIRE(list.begin() == list.end());
    REQUIRE(list.rbegin() == list.rend());
}

//============================================================================
// can insert multi items

TEST_CASE( "multi_skip_list/insert", "" )
{
    multi_skip_list<int> list;
    
    list.insert(10);
    REQUIRE(list.size() == 1);

    list.insert(10);
    REQUIRE(list.size() == 2);

    list.insert(10);
    REQUIRE(list.size() == 3);

    list.insert(11);
    REQUIRE(list.size() == 4);
    
    multi_skip_list<int>::iterator i = list.begin();
    REQUIRE(*i == 10); ++i;
    REQUIRE(*i == 10); ++i;
    REQUIRE(*i == 10); ++i;
    REQUIRE(*i == 11); ++i;
    REQUIRE(i == list.end());
}

TEST_CASE( "multi_skip_list/count", "" )
{
    multi_skip_list<int> list;
    REQUIRE(list.count(10) == 0);

    list.insert(10);
    REQUIRE(list.count(10) == 1);

    list.insert(10);
    REQUIRE(list.count(10) == 2);

    list.insert(10);
    REQUIRE(list.count(10) == 3);
}

TEST_CASE( "multi_skip_list/remove a multi", "" )
{
    multi_skip_list<int> list;
    
    list.insert(10);
    list.insert(11);
    list.insert(10);
    list.insert(11);
    REQUIRE(list.count(10) == 2);
    REQUIRE(list.count(11) == 2);

    REQUIRE(list.erase(10) == 2);
    REQUIRE(list.count(10) == 0);
    REQUIRE(list.count(11) == 2);

    REQUIRE(list.erase(11) == 2);
    REQUIRE(list.count(10) == 0);
    REQUIRE(list.count(11) == 0);
}

//============================================================================
// erase

TEST_CASE( "multi_skip_list/erase over multiple item boundary", "" )
{
    multi_skip_list<int> list;
    
    list.insert(11);
    list.insert(10);
    list.insert(11);
    list.insert(10);
    list.insert(11);

    multi_skip_list<int>::iterator start = ++list.begin();
    multi_skip_list<int>::iterator end   = start; ++++end;
    
    REQUIRE(std::distance(list.begin(), start) == 1);
    REQUIRE(std::distance(list.begin(), end) == 3);
    
    REQUIRE(*start == 10);
    REQUIRE(*end == 11);
    
    REQUIRE(list.size() == 5);
    list.erase(start,end);
    REQUIRE(list.size() == 3);
    
    multi_skip_list<int>::iterator i = list.begin();
    REQUIRE(*i == 10); ++i;
    REQUIRE(*i == 11); ++i;
}

TEST_CASE( "multi_skip_list/erase over multiple item boundary", "" )
{
    multi_skip_list<int> list;
    
    list.insert(11);
    list.insert(10);
    list.insert(11);
    list.insert(10);
    list.insert(11);
    REQUIRE(list.size() == 5);

    multi_skip_list<int>::iterator i      = ++++list.begin();
    multi_skip_list<int>::iterator result = list.erase(i);

    REQUIRE(*result == 11);
    REQUIRE(std::distance(list.begin(), result) == 2);
    REQUIRE(list.size() == 4);
    
    i = ++list.begin();
    result = list.erase(i);

    REQUIRE(*result == 11);
    REQUIRE(std::distance(list.begin(), result) == 1);
    REQUIRE(list.size() == 3);
    
    i = list.begin();
    result = list.erase(i);
    
    REQUIRE(*result == 11);
    REQUIRE(std::distance(list.begin(), result) == 0);
    REQUIRE(list.size() == 2);

    i = ++list.begin();
    result = list.erase(i);
    
    REQUIRE(result == list.end());
    REQUIRE(std::distance(list.begin(), result) == 1);
    REQUIRE(list.size() == 1);
    
    i = list.begin();
    result = list.erase(i);

    REQUIRE(result == list.end());
    REQUIRE(list.empty());
}

//============================================================================
// son of the mother of all tests

TEST_CASE( "multi_skip_list/comparison with multiset", "" )
{
    std::multiset<int> set;
    multi_skip_list<int> list;

    for (unsigned loops = 0; loops < 10; ++loops)
    {
        for (unsigned n = 0; n < 1000; ++n)
        {
            {
                int number = rand()%1000;
                set.insert(number);
                list.insert(number);
                //REQUIRE(set.count(number) == list.count(number));
            }
            {
                int number = rand()%100;
                set.insert(number);
                list.insert(number);
                //REQUIRE(set.count(number) == list.count(number));
            }
            REQUIRE(CheckEquality(list, set));
        }

        for (unsigned n = 0; n < 300; ++n)
        {
            {
                int number = rand()%1000;
                //REQUIRE(set.count(number) == list.count(number));
                size_t set_erase  = set.erase(number);
                size_t list_erase = list.erase(number);
                REQUIRE(set_erase == list_erase);
            }
            {
                int number = rand()%100;
                //REQUIRE(set.count(number) == list.count(number));
                size_t set_erase  = set.erase(number);
                size_t list_erase = list.erase(number);
                REQUIRE(set_erase == list_erase);
            }
            REQUIRE(CheckEquality(list, set));
        }
    }
}

//============================================================================
// lower_bound

template <typename T, typename CONTAINER>
bool LowerBoundTest(const T &value, CONTAINER &container, typename CONTAINER::difference_type advance)
{
    typename CONTAINER::iterator i = container.lower_bound(value);
    //unsigned distance = (unsigned)std::distance(container.begin(), i); 
    //REQUIRE(distance == ((unsigned)advance));
    return std::distance(container.begin(), i) == advance;
}
template <typename T, typename CONTAINER>
bool LowerBoundTest(const T &value, const CONTAINER &container, typename CONTAINER::difference_type advance)
{
    return std::distance(container.begin(), container.lower_bound(value)) == advance;
}

TEST_CASE( "multi_skip_list/lower_bound/with empty list", "" )
{
    multi_skip_list<int> list;
    
    REQUIRE(list.lower_bound(0) == list.end());
    REQUIRE(list.lower_bound(1) == list.end());
    REQUIRE(list.lower_bound(100) == list.end());
}

TEST_CASE( "multi_skip_list/lower_bound/comparison with multiset", "" )
{
    std::multiset<int> set; // we use multiset as a comparison of behaviour
    set.insert(5);
    set.insert(7);
    set.insert(7);
    set.insert(11);
    set.insert(11);
    set.insert(21);

    multi_skip_list<int> list(set.begin(), set.end());

    REQUIRE(LowerBoundTest(5,  set, 0)); REQUIRE(LowerBoundTest(5,  list, 0));
    REQUIRE(LowerBoundTest(7,  set, 1)); REQUIRE(LowerBoundTest(7,  list, 1));
    REQUIRE(LowerBoundTest(11, set, 3)); REQUIRE(LowerBoundTest(11, list, 3));
    REQUIRE(LowerBoundTest(21, set, 5)); REQUIRE(LowerBoundTest(21, list, 5));

    REQUIRE(LowerBoundTest(0,  set, 0)); REQUIRE(LowerBoundTest(0,  list, 0));
    REQUIRE(LowerBoundTest(4,  set, 0)); REQUIRE(LowerBoundTest(4,  list, 0));
    REQUIRE(LowerBoundTest(6,  set, 1)); REQUIRE(LowerBoundTest(6,  list, 1));
    REQUIRE(LowerBoundTest(22, set, 6)); REQUIRE(LowerBoundTest(22, list, 6));
    
    const multi_skip_list<int> &clist = list;
    REQUIRE(LowerBoundTest(5,  clist, 0));
    REQUIRE(LowerBoundTest(7,  clist, 1));
    REQUIRE(LowerBoundTest(11, clist, 3));
    REQUIRE(LowerBoundTest(21, clist, 5));
    
    REQUIRE(LowerBoundTest(0,  clist, 0));
    REQUIRE(LowerBoundTest(4,  clist, 0));
    REQUIRE(LowerBoundTest(6,  clist, 1));
    REQUIRE(LowerBoundTest(22, clist, 6));
}

//============================================================================
// upper_bound

template <typename T, typename CONTAINER>
bool UpperBoundTest(const T &value, CONTAINER &container, size_t advance)
{
    typename CONTAINER::iterator i = container.begin();
    std::advance(i, advance);
    return (container.upper_bound(value) == i);
}
template <typename T, typename CONTAINER>
bool UpperBoundTest(const T &value, const CONTAINER &container, size_t advance)
{
    typename CONTAINER::const_iterator i = container.begin();
    std::advance(i, advance);
    return (container.upper_bound(value) == i);
}

TEST_CASE( "multi_skip_list/upper_bound/with empty list", "" )
{
    multi_skip_list<int> list;
    REQUIRE(list.upper_bound(0) == list.end());
    REQUIRE(list.upper_bound(1) == list.end());
    REQUIRE(list.upper_bound(100) == list.end());
}

TEST_CASE( "multi_skip_list/upper_bound/comparison with multiset", "" )
{
    std::multiset<int> set; // we use multiset as a comparison of behaviour
    set.insert(5);
    set.insert(7);
    set.insert(7);
    set.insert(11);
    set.insert(11);
    set.insert(21);
    
    multi_skip_list<int> list(set.begin(), set.end());
    REQUIRE(UpperBoundTest(5,  set, 1)); REQUIRE(UpperBoundTest(5,  list, 1));
    REQUIRE(UpperBoundTest(7,  set, 3)); REQUIRE(UpperBoundTest(7,  list, 3));
    REQUIRE(UpperBoundTest(11, set, 5)); REQUIRE(UpperBoundTest(11, list, 5));
    REQUIRE(UpperBoundTest(21, set, 6)); REQUIRE(UpperBoundTest(21, list, 6));
    
    REQUIRE(UpperBoundTest(0,  set, 0)); REQUIRE(UpperBoundTest(0,  list, 0));
    REQUIRE(UpperBoundTest(4,  set, 0)); REQUIRE(UpperBoundTest(4,  list, 0));
    REQUIRE(UpperBoundTest(6,  set, 1)); REQUIRE(UpperBoundTest(6,  list, 1));
    REQUIRE(UpperBoundTest(8,  set, 3)); REQUIRE(UpperBoundTest(8,  list, 3));
    REQUIRE(UpperBoundTest(20, set, 5)); REQUIRE(UpperBoundTest(20, list, 5));
    REQUIRE(UpperBoundTest(22, set, 6)); REQUIRE(UpperBoundTest(22, list, 6));
    
    const multi_skip_list<int> &clist = list;
    REQUIRE(UpperBoundTest(5,  clist, 1));
    REQUIRE(UpperBoundTest(7,  clist, 3));
    REQUIRE(UpperBoundTest(11, clist, 5));
    REQUIRE(UpperBoundTest(21, clist, 6));
    REQUIRE(UpperBoundTest(0,  clist, 0));
    REQUIRE(UpperBoundTest(4,  clist, 0));
    REQUIRE(UpperBoundTest(6,  clist, 1));
    REQUIRE(UpperBoundTest(8,  clist, 3));
    REQUIRE(UpperBoundTest(20, clist, 5));
    REQUIRE(UpperBoundTest(22, clist, 6));
}

//============================================================================
// equal_range

template <typename T, typename CONTAINER>
bool EqualRangeTest(const T &value, CONTAINER &container)
{
    typedef typename CONTAINER::iterator iterator;
    std::pair<iterator,iterator> range = container.equal_range(value);
    return range.first == container.lower_bound(value)
        && range.second == container.upper_bound(value);
}
template <typename T, typename CONTAINER>
bool EqualRangeTest(const T &value, const CONTAINER &container)
{
    typedef typename CONTAINER::const_iterator iterator;
    std::pair<iterator,iterator> range = container.equal_range(value);
    return range.first == container.lower_bound(value)
        && range.second == container.upper_bound(value);
}

TEST_CASE( "multi_skip_list/equal_range/with empty list", "" )
{
    multi_skip_list<int> list;
    std::pair<multi_skip_list<int>::iterator, multi_skip_list<int>::iterator> invalid = std::make_pair(list.end(), list.end());
    REQUIRE(list.equal_range(0) == invalid);
    REQUIRE(list.equal_range(1) == invalid);
    REQUIRE(list.equal_range(100) == invalid);
}

TEST_CASE( "multi_skip_list/equal_range/comparison with multiset", "" )
{
    std::multiset<int> set; // we use multiset as a comparison of behaviour
    set.insert(5);
    set.insert(7);
    set.insert(7);
    set.insert(11);
    set.insert(11);
    set.insert(21);
    
    multi_skip_list<int> list(set.begin(), set.end());
    REQUIRE(EqualRangeTest(5,  set)); REQUIRE(EqualRangeTest(5,  list));
    REQUIRE(EqualRangeTest(7,  set)); REQUIRE(EqualRangeTest(7,  list));
    REQUIRE(EqualRangeTest(11, set)); REQUIRE(EqualRangeTest(11, list));
    REQUIRE(EqualRangeTest(21, set)); REQUIRE(EqualRangeTest(21, list));
    
    REQUIRE(EqualRangeTest(0,  set)); REQUIRE(EqualRangeTest(0,  list));
    REQUIRE(EqualRangeTest(4,  set)); REQUIRE(EqualRangeTest(4,  list));
    REQUIRE(EqualRangeTest(6,  set)); REQUIRE(EqualRangeTest(6,  list));
    REQUIRE(EqualRangeTest(8,  set)); REQUIRE(EqualRangeTest(8,  list));
    REQUIRE(EqualRangeTest(20, set)); REQUIRE(EqualRangeTest(20, list));
    REQUIRE(EqualRangeTest(22, set)); REQUIRE(EqualRangeTest(22, list));
    
    const multi_skip_list<int> &clist = list;
    REQUIRE(EqualRangeTest(5,  clist));
    REQUIRE(EqualRangeTest(7,  clist));
    REQUIRE(EqualRangeTest(11, clist));
    REQUIRE(EqualRangeTest(21, clist));
    REQUIRE(EqualRangeTest(0,  clist));
    REQUIRE(EqualRangeTest(4,  clist));
    REQUIRE(EqualRangeTest(6,  clist));
    REQUIRE(EqualRangeTest(8,  clist));
    REQUIRE(EqualRangeTest(20, clist));
    REQUIRE(EqualRangeTest(22, clist));
}
