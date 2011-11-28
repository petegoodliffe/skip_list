//============================================================================
// benchmark.cpp
// Copyright (c) 2011 Pete Goodliffe. All rights reserved
//============================================================================

// MSVS quietens warnings
#pragma warning (disable : 4068 ) /* disable unknown pragma warnings */

#include "skip_list.h"

#include <set>
#include <vector>

#include "catch.hpp"

using goodliffe::skip_list;
using goodliffe::detail::skip_list_impl;

TEST_CASE( "skip_list/benchmark/smoketest", "" )
{
    //REQUIRE(false);
}

//============================================================================
// performance test
//============================================================================

#include "get_time.h"

TEST_CASE( "skip_list/performance/insert by value", "" )
{
    std::vector<int> data;
    for (int n = 0; n < 10000; ++n)
        data.push_back(rand());
    
    {
        long start = get_time_ms();
        std::set<int> set;
        for (std::vector<int>::iterator i = data.begin(); i != data.end(); ++i)
            set.insert(*i);
        long end = get_time_ms();
        
        long elapsed = end-start;
        fprintf(stderr, "std::set insert by value: %ld\n", elapsed);
    }
    
    {
        long start = get_time_ms();
        skip_list<int> list;
        for (std::vector<int>::iterator i = data.begin(); i != data.end(); ++i)
            list.insert(*i);
        long end = get_time_ms();
        
        long elapsed = end-start;
        fprintf(stderr, "skip_list insert by value: %ld\n", elapsed);
        
        list.dump(std::cerr);
    }
}

TEST_CASE( "skip_list/performance/iterate through", "" )
{
    std::vector<int> data;
    for (int n = 0; n < 10000; ++n)
        data.push_back(rand());
    
    {
        std::set<int> set;
        for (std::vector<int>::iterator i = data.begin(); i != data.end(); ++i)
            set.insert(*i);
        
        long start = get_time_ms();
        for (unsigned n = 0 ; n < 100; ++n)
            for (std::set<int>::iterator i = set.begin(); i != set.end(); ++i);
        long end = get_time_ms();
        
        long elapsed = end-start;
        fprintf(stderr, "std::set iterate forwards: %ld\n", elapsed);
    }
    
    {
        skip_list<int> list;
        for (std::vector<int>::iterator i = data.begin(); i != data.end(); ++i)
            list.insert(*i);
        
        long start = get_time_ms();
        for (unsigned n = 0 ; n < 100; ++n)
            for (skip_list<int>::iterator i = list.begin(); i != list.end(); ++i);
        long end = get_time_ms();
        
        long elapsed = end-start;
        fprintf(stderr, "skip_list iterator forwards: %ld\n", elapsed);
    }
}

TEST_CASE( "skip_list/performance/find", "" )
{
    std::vector<int> data;
    for (int n = 0; n < 10000; ++n) data.push_back(rand());
    
    {
        std::set<int> set;
        for (std::vector<int>::iterator i = data.begin(); i != data.end(); ++i)
            set.insert(*i);
        
        long start = get_time_ms();
        for (int n = 0 ; n < 10000; ++n)
            (void)set.find(n);
        long end = get_time_ms();
        
        long elapsed = end-start;
        fprintf(stderr, "std::set iterate forwards: %ld\n", elapsed);
    }
    
    {
        skip_list<int> list;
        for (std::vector<int>::iterator i = data.begin(); i != data.end(); ++i)
            list.insert(*i);
        
        long start = get_time_ms();
        for (int n = 0 ; n < 100; ++n)
            (void)list.find(n);
        long end = get_time_ms();
        
        long elapsed = end-start;
        fprintf(stderr, "skip_list iterator forwards: %ld\n", elapsed);
    }
}
