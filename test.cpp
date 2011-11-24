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

struct Struct { int i; float f; };

TEST_CASE( "skip_list/can be constructed and destroyed", "" )
{
    skip_list<int>    sl_int;
    skip_list<float>  sl_float;
    skip_list<Struct> sl_struct;
}

