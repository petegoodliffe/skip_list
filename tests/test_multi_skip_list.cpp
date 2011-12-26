//============================================================================
// test_multi_skip_list.cpp
// Copyright (c) 2011 Pete Goodliffe. All rights reserved
//============================================================================

#include "skip_list.h"

#define CATCH_CONFIG_NO_STREAM_REDIRECTION 1
#include "catch.hpp"
#include "test_types.h"

#include <set>

using goodliffe::multi_skip_list;

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

    list.erase(10);
    REQUIRE(list.count(10) == 1);
    REQUIRE(list.count(11) == 2);

    list.erase(11);
    REQUIRE(list.count(10) == 1);
    REQUIRE(list.count(11) == 1);

    list.erase(10);
    REQUIRE(list.count(10) == 0);
    REQUIRE(list.count(11) == 1);

    list.erase(11);
    REQUIRE(list.count(10) == 0);
    REQUIRE(list.count(11) == 0);
}

TEST_CASE( "multi_skip_list/erase over multie item boundary", "" )
{
    multi_skip_list<int> list;
    
    list.insert(11);
    list.insert(10);
    list.insert(11);
    list.insert(10);
    list.insert(11);
    
    multi_skip_list<int>::iterator start = list.begin()++++;
    multi_skip_list<int>::iterator end   = start++++;
    
    REQUIRE(list.size() == 5);
    list.erase(start,end);
    REQUIRE(list.size() == 3);
    
    multi_skip_list<int>::iterator i = list.begin();
    REQUIRE(*i == 10); ++i;
    REQUIRE(*i == 11); ++i;
}

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
            }
            {
                int number = rand()%100;
                set.insert(number);
                list.insert(number);
            }
            REQUIRE(CheckEquality(list, set));
        }
        
        for (unsigned n = 0; n < 300; ++n)
        {
            {
                int number = rand()%1000;
                set.erase(number);
                list.erase(number);
            }
            {
                int number = rand()%100;
                set.erase(number);
                list.erase(number);
            }
            REQUIRE(CheckEquality(list, set));
        }
    }
}
