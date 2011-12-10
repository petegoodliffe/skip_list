//============================================================================
// test_random_access.cpp
// Copyright (c) 2011 Pete Goodliffe. All rights reserved
//============================================================================

#include "skip_list.h"

#include "catch.hpp"
#include "test_types.h"

using goodliffe::random_access_skip_list;

TEST_CASE( "random_access_skip_list/smoketest", "" )
{
    //REQUIRE(false);
}

TEST_CASE( "random_access_skip_list/is constructable", "" )
{
    random_access_skip_list<int>    sl_int;
    random_access_skip_list<float>  sl_float;
    random_access_skip_list<Struct> sl_struct;
    
    REQUIRE(true);
}

TEST_CASE( "random_access_skip_list/can call basic methods from skip_list", "" )
{
    const random_access_skip_list<int> list;
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

TEST_CASE( "random_access_skip_list/add items", "" )
{
    random_access_skip_list<int> list;
    list.dump(std::cerr);
    REQUIRE(list.size() == 0);

    list.insert(0);
    REQUIRE(list.size() == 1);
    list.dump(std::cerr);
    REQUIRE(list[0] == 0);

    list.insert(1);
    REQUIRE(list.size() == 2);
    list.dump(std::cerr);
    REQUIRE(list[0] == 0);
    REQUIRE(list[1] == 1);
    
    list.insert(2);
    REQUIRE(list.size() == 3);
    list.dump(std::cerr);
    REQUIRE(list[0] == 0);
    REQUIRE(list[1] == 1);
    REQUIRE(list[2] == 2);
    
    list.insert(3);
    REQUIRE(list.size() == 4);
    list.dump(std::cerr);
    REQUIRE(list[0] == 0);
    REQUIRE(list[1] == 1);
    REQUIRE(list[2] == 2);
    REQUIRE(list[3] == 3);
    
    list.insert(4);
    REQUIRE(list.size() == 5);
    list.dump(std::cerr);
    REQUIRE(list[0] == 0);
    REQUIRE(list[1] == 1);
    REQUIRE(list[2] == 2);
    REQUIRE(list[3] == 3);
    REQUIRE(list[4] == 4);
    
    list.insert(5);
    REQUIRE(list.size() == 6);
    list.dump(std::cerr);
    REQUIRE(list[0] == 0);
    REQUIRE(list[1] == 1);
    REQUIRE(list[2] == 2);
    REQUIRE(list[3] == 3);
    REQUIRE(list[4] == 4);
    
    list.dump(std::cerr);
}

TEST_CASE( "random_access_skip_list/non members", "" )
{
    random_access_skip_list<int> list1, list2;
    list1.insert(1);
    list2.insert(2);
    
    REQUIRE(list1 != list2);
    REQUIRE_FALSE(list1 == list2);
}

// TODO: allocation test

