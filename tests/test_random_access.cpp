//============================================================================
// test_random_access.cpp
// Copyright (c) 2011 Pete Goodliffe. All rights reserved
//============================================================================

#include "skip_list.h"

#include "catch.hpp"

using goodliffe::random_access_skip_list;

TEST_CASE( "random_access_skip_list/smoketest", "" )
{
    //REQUIRE(false);
}

TEST_CASE( "random_access_skip_list/is constructable", "" )
{
    random_access_skip_list<int> list;
}
