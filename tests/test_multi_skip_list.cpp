//============================================================================
// test_multi_skip_list.cpp
// Copyright (c) 2011 Pete Goodliffe. All rights reserved
//============================================================================

#include "skip_list.h"

#define CATCH_CONFIG_NO_STREAM_REDIRECTION 1
#include "catch.hpp"
#include "test_types.h"

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
}
