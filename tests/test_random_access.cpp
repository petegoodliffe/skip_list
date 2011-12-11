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
    REQUIRE(list.size() == 0);

    list.insert(0);
    REQUIRE(list.size() == 1);
    REQUIRE(list[0] == 0);

    list.insert(1);
    REQUIRE(list.size() == 2);
    REQUIRE(list[0] == 0);
    REQUIRE(list[1] == 1);
    
    list.insert(2);
    REQUIRE(list.size() == 3);
    REQUIRE(list[0] == 0);
    REQUIRE(list[1] == 1);
    REQUIRE(list[2] == 2);
    
    list.insert(3);
    REQUIRE(list.size() == 4);
    REQUIRE(list[0] == 0);
    REQUIRE(list[1] == 1);
    REQUIRE(list[2] == 2);
    REQUIRE(list[3] == 3);
    
    list.insert(4);
    REQUIRE(list.size() == 5);
    REQUIRE(list[0] == 0);
    REQUIRE(list[1] == 1);
    REQUIRE(list[2] == 2);
    REQUIRE(list[3] == 3);
    REQUIRE(list[4] == 4);
    
    list.insert(5);
    REQUIRE(list.size() == 6);
    REQUIRE(list[0] == 0);
    REQUIRE(list[1] == 1);
    REQUIRE(list[2] == 2);
    REQUIRE(list[3] == 3);
    REQUIRE(list[4] == 4);
}

TEST_CASE( "random_access_skip_list/indexing", "" )
{
    for (unsigned n = 0; n < 10; ++n)
    {
        unsigned size = unsigned(rand()) % 300;
        std::vector<int> data;
        random_access_skip_list<int> list;
        FillWithRandomData(size, data);
        for (unsigned m = 0 ; m < size; ++m)
            list.insert(data[m]);
        
        SortVectorAndRemoveDuplicates(data);
        
        for (unsigned m = 0 ; m < data.size(); ++m)
        {
            REQUIRE(list[m] == data[m]);
        }

        // Check that clear puts indexing back to right state so subsequent
        // insertions are correct
        list.clear();
        REQUIRE(list.size() == 0);
        REQUIRE(list.empty());

        for (unsigned m = 0 ; m < data.size(); ++m)
            list.insert(data[data.size()-1-m]);
        
        for (unsigned m = 0 ; m < data.size(); ++m)
        {
            REQUIRE(list[m] == data[m]);
        }
    }
}

TEST_CASE( "random_access_skip_list/iterator_at", "" )
{
    std::vector<int> data;
    FillWithRandomData(1000, data);
    
    random_access_skip_list<int> list(data.begin(), data.end());
    
    SortVectorAndRemoveDuplicates(data);
    
    for (unsigned n = 0; n < list.size(); n += 7)
    {
        REQUIRE(*list.iterator_at(n) == data[n]);
    }
}

TEST_CASE( "random_access_skip_list/erase/maintains indexes/1", "" )
{
    random_access_skip_list<int> list;
    for (int n = 0; n < 9; ++n)
        list.insert(n);
    
    list.erase_at(1);
    REQUIRE(list[0] == 0);    REQUIRE(list[1] == 2);    REQUIRE(list[2] == 3);
    REQUIRE(list[3] == 4);    REQUIRE(list[4] == 5);    REQUIRE(list[7] == 8);
}

TEST_CASE( "random_access_skip_list/erase/maintains indexes/2", "" )
{
    random_access_skip_list<int> list;
    for (int n = 0; n < 9; ++n)
        list.insert(n);
    
    list.erase_at(2);
    REQUIRE(list[0] == 0);    REQUIRE(list[1] == 1);    REQUIRE(list[2] == 3);
    REQUIRE(list[3] == 4);    REQUIRE(list[4] == 5);    REQUIRE(list[7] == 8);
}

TEST_CASE( "random_access_skip_list/erase/maintains indexes/3", "" )
{
    random_access_skip_list<int> list;
    for (int n = 0; n < 9; ++n)
        list.insert(n);

    list.erase_at(3);
    REQUIRE(list[0] == 0);    REQUIRE(list[1] == 1);    REQUIRE(list[2] == 2);
    REQUIRE(list[3] == 4);    REQUIRE(list[4] == 5);    REQUIRE(list[7] == 8);
}

TEST_CASE( "random_access_skip_list/erase/maintains indexes/4", "" )
{
    random_access_skip_list<int> list;
    for (int n = 0; n < 9; ++n)
        list.insert(n);
    
    list.erase_at(4);
    REQUIRE(list[0] == 0);    REQUIRE(list[1] == 1);    REQUIRE(list[2] == 2);
    REQUIRE(list[3] == 3);    REQUIRE(list[4] == 5);    REQUIRE(list[7] == 8);
}

TEST_CASE( "random_access_skip_list/erase/maintains indexes", "" )
{
    std::vector<int> data;
    FillWithRandomData(1000, data);
    
    random_access_skip_list<int> list(data.begin(), data.end());

    SortVectorAndRemoveDuplicates(data);

    for (unsigned n = 0 ; n < 20; ++n)
    {
        unsigned index = unsigned(rand()) % unsigned(list.size());
        fprintf(stderr, "  remove %u/%u\n", index, (unsigned)list.size());
        list.erase_at(index);
        data.erase(data.begin()+index);
    }
    
    REQUIRE(CheckForwardIteration(list));
    REQUIRE(CheckBackwardIteration(list));
    
    REQUIRE(list.size() == data.size());
    /*
    for (unsigned n = 0; n < list.size(); ++n)
    {
        fprintf(stderr, "  %u/%u: %d==%d %s\n", n, (unsigned)list.size(), list[n], data[n], list[n] == data[n]?"":"NO");
    }
     */
    REQUIRE(CheckEquality(list, data));
}

TEST_CASE( "random_access_skip_list/non members", "" )
{
    random_access_skip_list<int> list1, list2;
    list1.insert(1);
    list2.insert(2);
    
    REQUIRE(list1 != list2);
    REQUIRE_FALSE(list1 == list2);
    
    REQUIRE(list1 < list2);
    REQUIRE(list1 <= list2);
    REQUIRE_FALSE(list1 > list2);
    REQUIRE_FALSE(list1 >= list2);
}

// TODO: allocation test

