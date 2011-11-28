//============================================================================
// benchmark.cpp
// Copyright (c) 2011 Pete Goodliffe. All rights reserved
//============================================================================

// MSVS quietens warnings
#pragma warning (disable : 4068 ) /* disable unknown pragma warnings */

#include "skip_list.h"

#include "get_time.h"

#include <set>
#include <list>
#include <vector>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>

#if BENCHMARK_WITH_MAIN
#define CATCH_CONFIG_MAIN
#endif 

#include "catch.hpp"

using goodliffe::skip_list;

TEST_CASE( "skip_list/benchmark/smoketest", "" )
{
    //REQUIRE(false);
}

//============================================================================

struct Benchmark
{
    std::string name;
    long        ms_vector;
    long        ms_set;
    long        ms_list;
    long        ms_skip_list;
    
    Benchmark()
        : name(), ms_vector(-1), ms_set(-1), ms_list(-1), ms_skip_list(-1) {}
    Benchmark(const std::string &name_)
        : name(name_), ms_vector(-1), ms_set(-1), ms_list(-1), ms_skip_list(-1) {}
};

long TimeExecutionOf(const boost::function<void()> &f);
void FillWithRandomData(size_t size, std::vector<int> &data);
void FillWithOrderedData(size_t size, std::vector<int> &data);
void FillWithReverseOrderedData(size_t size, std::vector<int> &data);

//============================================================================

int allocator_bytes_allocated = 0;
int allocator_objects_constructed = 0;

template <typename T = int>
struct TestingAllocator : public std::allocator<T>
{
    TestingAllocator() {}
    template <class OTHER>
    TestingAllocator(OTHER &other) : alloc(other.alloc) {}
    
    template <typename OTHER>
    struct rebind { typedef TestingAllocator<OTHER> other; };
    
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    
    pointer allocate(size_type n, std::allocator<void>::const_pointer hint=0)
    {
        allocator_bytes_allocated += sizeof(T)*n;
        return std::allocator<T>::allocate(n,hint);
    }
    void deallocate(pointer p, size_type n)
    {
        allocator_bytes_allocated -= sizeof(T)*n;
        return std::allocator<T>::deallocate(p,n);
    }
    void construct(pointer p, const_reference val)
    {
        allocator_objects_constructed++;
        return std::allocator<T>::construct(p, val);
    }
    void destroy(pointer p)
    {
        allocator_objects_constructed--;
        return std::allocator<T>::destroy(p);
    }
    
    std::allocator<T> alloc;
};

//============================================================================

long TimeExecutionOf(const boost::function<void()> &f)
{
    const long start = get_time_ms();
    f();
    const long end = get_time_ms();
    return end-start;
}

//============================================================================

void FillWithRandomData(size_t size, std::vector<int> &data)
{
    data.clear();
    for (size_t n = 0; n < size; ++n) data.push_back(rand());
}

void FillWithOrderedData(size_t size, std::vector<int> &data)
{
    data.clear();
    for (size_t n = 0; n < size; ++n) data.push_back(int(n));
}

void FillWithReverseOrderedData(size_t size, std::vector<int> &data)
{
    data.clear();
    for (size_t n = size; n; --n) data.push_back(int(n));
}

//============================================================================
// test methods

template <typename CONTAINER>
bool ContainerIsInOrder(const CONTAINER &container)
{
    REQUIRE_FALSE(container.empty());
    int last_value = *container.begin();
    for (typename CONTAINER::const_iterator i = ++container.begin(); i != container.end(); ++i)
    {
        //REQUIRE(*i > last_value);
        if (*i < last_value) return false;
        last_value = *i;
    }
    return true;
}
template <typename CONTAINER>
void InsertByValue(const std::vector<int> *data, CONTAINER *container)
{
    for (std::vector<int>::const_iterator i = data->begin(); i != data->end(); ++i)
        container->insert(*i);
}
template <typename CONTAINER>
void InsertByValueAtRightPlace(const std::vector<int> *data, CONTAINER *container)
{
    for (std::vector<int>::const_iterator i = data->begin(); i != data->end(); ++i)
    {
        if (container->empty())
            container->push_back(*i);
        else
        {
            typename CONTAINER::iterator insert = container->begin();
            while (insert != container->end() && *insert < *i)
                ++insert;
            container->insert(insert, *i);
        }
    }
}
template <typename CONTAINER>
void PushBack(const std::vector<int> *data, CONTAINER *container)
{
    for (std::vector<int>::const_iterator i = data->begin(); i != data->end(); ++i)
        container->push_back(*i);
}
template <typename CONTAINER>
void IterateForwardsThrough(CONTAINER *container)
{
    for (unsigned n = 0; n < 100; ++n)
        for (typename CONTAINER::iterator i = container->begin(); i != container->end(); ++i);
}
template <typename CONTAINER>
void IterateBackwardsThrough(CONTAINER *container)
{
    for (unsigned n = 0; n < 100; ++n)
        for (typename CONTAINER::reverse_iterator i = container->rbegin(); i != container->rend(); ++i);
}
template <typename CONTAINER>
void Find(const CONTAINER *container)
{
    for (int n = 0 ; n < int(container->size()); ++n)
    {
        typename CONTAINER::const_iterator insert = container->find(n);
        REQUIRE(std::distance(container->begin(), insert) == n);
    }
}
template <typename CONTAINER>
void FindManually(const CONTAINER *container)
{
    for (int n = 0 ; n < int(container->size()); ++n)
    {
        typename CONTAINER::const_iterator insert = container->begin();
        while (insert != container->end() && *insert < n)
            ++insert;
        REQUIRE(std::distance(container->begin(), insert) == n);
    }
}

//============================================================================
// performance test
//============================================================================

Benchmark InsertData(std::vector<int> &data, const std::string &name);
Benchmark InsertData(std::vector<int> &data, const std::string &name)
{
    std::set<int>    std_set;
    std::list<int>   std_list;
    std::vector<int> std_vector;
    skip_list<int>   skip_list;

    Benchmark benchmark("insert data: "+name);    

    benchmark.ms_set        = TimeExecutionOf(boost::bind(&InsertByValue<std::set<int> >, &data, &std_set));
    benchmark.ms_list       = TimeExecutionOf(boost::bind(&InsertByValueAtRightPlace<std::list<int> >, &data, &std_list));
    benchmark.ms_vector     = TimeExecutionOf(boost::bind(&InsertByValueAtRightPlace<std::vector<int> >, &data, &std_vector));
    benchmark.ms_skip_list  = TimeExecutionOf(boost::bind(&InsertByValue<goodliffe::skip_list<int> >, &data, &skip_list));
    
    REQUIRE(std_list.size() == data.size());
    REQUIRE(std_set.size()  == data.size());

    // Sanity test
    //REQUIRE(ContainerIsInOrder(std_set));
    //REQUIRE(ContainerIsInOrder(std_list));
    //REQUIRE(ContainerIsInOrder(std_vector));
    //REQUIRE(ContainerIsInOrder(skip_list));

    return benchmark;
}

Benchmark InsertRandomOrderedData();
Benchmark InsertRandomOrderedData()
{
    std::vector<int> data;
    FillWithRandomData(10000, data);
    return InsertData(data, "random order");
}

Benchmark InsertOrderedData();
Benchmark InsertOrderedData()
{
    std::vector<int> data;
    FillWithOrderedData(10000, data);
    return InsertData(data, "ordered");
}

Benchmark InsertReverseOrderedData();
Benchmark InsertReverseOrderedData()
{
    std::vector<int> data;
    FillWithReverseOrderedData(10000, data);
    return InsertData(data, "reverse");
}
Benchmark IterateForwards();
Benchmark IterateForwards()
{
    std::vector<int> data;
    FillWithRandomData(10000, data);
    
    std::set<int>    std_set(data.begin(), data.end());
    std::list<int>   std_list(std_set.begin(), std_set.end());   // use set to ensure order
    std::vector<int> std_vector(std_set.begin(), std_set.end()); // use set to ensure order
    skip_list<int>   skip_list(data.begin(), data.end());
    
    Benchmark benchmark("interate forwards");    
    
    benchmark.ms_set        = TimeExecutionOf(boost::bind(&IterateForwardsThrough<std::set<int> >, &std_set));
    benchmark.ms_list       = TimeExecutionOf(boost::bind(&IterateForwardsThrough<std::list<int> >, &std_list));
    benchmark.ms_vector     = TimeExecutionOf(boost::bind(&IterateForwardsThrough<std::vector<int> >, &std_vector));
    benchmark.ms_skip_list  = TimeExecutionOf(boost::bind(&IterateForwardsThrough<goodliffe::skip_list<int> >, &skip_list));
    /*
    {
        std::set<int> set(data.begin(), data.end());
        for (std::vector<int>::iterator i = data.begin(); i != data.end(); ++i)
            set.insert(*i);
        
        long start = get_time_ms();
        for (unsigned n = 0 ; n < 100; ++n)
            for (std::set<int>::iterator i = std_set.begin(); i != std_set.end(); ++i);
        long end = get_time_ms();
        
        long elapsed = end-start;
        fprintf(stderr, "std::set iterate forwards: %ld\n", elapsed);
    }
    
    {
        goodliffe::skip_list<int> list(data.begin(), data.end());
        for (std::vector<int>::iterator i = data.begin(); i != data.end(); ++i)
            list.insert(*i);
        
        long start = get_time_ms();
        for (unsigned n = 0 ; n < 100; ++n)
            for (goodliffe::skip_list<int>::iterator i = skip_list.begin(); i != skip_list.end(); ++i);
        long end = get_time_ms();
        
        long elapsed = end-start;
        fprintf(stderr, "skip_list iterator forwards: %ld\n", elapsed);
    }*/
    return benchmark;
}

Benchmark IterateBackwards();
Benchmark IterateBackwards()
{
    std::vector<int> data;
    FillWithRandomData(10000, data);

    std::set<int>    std_set(data.begin(), data.end());
    std::list<int>   std_list(std_set.begin(), std_set.end());   // use set to ensure order
    std::vector<int> std_vector(std_set.begin(), std_set.end()); // use set to ensure order
    skip_list<int>   skip_list(data.begin(), data.end());
    
    Benchmark benchmark("interate backwards");    
    
    benchmark.ms_set        = TimeExecutionOf(boost::bind(&IterateBackwardsThrough<std::set<int> >, &std_set));
    benchmark.ms_list       = TimeExecutionOf(boost::bind(&IterateBackwardsThrough<std::list<int> >, &std_list));
    benchmark.ms_vector     = TimeExecutionOf(boost::bind(&IterateBackwardsThrough<std::vector<int> >, &std_vector));
    benchmark.ms_skip_list  = TimeExecutionOf(boost::bind(&IterateBackwardsThrough<goodliffe::skip_list<int> >, &skip_list));

    return benchmark;
}

Benchmark Find();
Benchmark Find()
{
    std::vector<int> data;
    FillWithOrderedData(10000, data);
    
    std::set<int>    std_set(data.begin(), data.end());
    std::list<int>   std_list(std_set.begin(), std_set.end());   // use set to ensure order
    std::vector<int> std_vector(std_set.begin(), std_set.end()); // use set to ensure order
    skip_list<int>   skip_list(data.begin(), data.end());
    
    Benchmark benchmark("find");    
    
    benchmark.ms_set        = TimeExecutionOf(boost::bind(&Find<std::set<int> >, &std_set));
    benchmark.ms_list       = TimeExecutionOf(boost::bind(&FindManually<std::list<int> >, &std_list));
    benchmark.ms_vector     = TimeExecutionOf(boost::bind(&FindManually<std::vector<int> >, &std_vector));
    benchmark.ms_skip_list  = TimeExecutionOf(boost::bind(&Find<goodliffe::skip_list<int> >, &skip_list));
    
    return benchmark;
}


Benchmark Allocation();
Benchmark Allocation()
{
    std::vector<int> data;
    FillWithRandomData(10000, data);
    
    Benchmark benchmark("allcoations");
    
    /*
    std::set<int>    std_set(data.begin(), data.end());
    std::list<int>   std_list(std_set.begin(), std_set.end());   // use set to ensure order
    std::vector<int> std_vector(std_set.begin(), std_set.end()); // use set to ensure order
    skip_list<int>   skip_list(data.begin(), data.end());
    */

    {
        allocator_bytes_allocated     = 0;
        allocator_objects_constructed = 0;
        std::set<int,std::less<int>,TestingAllocator<int> > std_set(data.begin(), data.end());
        benchmark.ms_set = allocator_bytes_allocated;
    }
    REQUIRE(allocator_bytes_allocated == 0);
    //REQUIRE(allocator_objects_constructed == 0);

    {
        allocator_bytes_allocated     = 0;
        allocator_objects_constructed = 0;
        std::list<int,TestingAllocator<int> > std_list(data.begin(), data.end());
        benchmark.ms_list = allocator_bytes_allocated;
    }
    REQUIRE(allocator_bytes_allocated == 0);
    
    {
        allocator_bytes_allocated     = 0;
        allocator_objects_constructed = 0;
        std::vector<int,TestingAllocator<int> > std_vector(data.begin(), data.end());
        benchmark.ms_vector = allocator_bytes_allocated;
    }
    REQUIRE(allocator_bytes_allocated == 0);
    
    {
        allocator_bytes_allocated     = 0;
        allocator_objects_constructed = 0;
        skip_list<int,std::less<int>,TestingAllocator<int> > skip_list(data.begin(), data.end());
        benchmark.ms_skip_list = allocator_bytes_allocated;
    }
    REQUIRE(allocator_bytes_allocated == 0);

    return benchmark;
}

//============================================================================

void Progress();
void Progress() { fprintf(stderr, "."); }

TEST_CASE( "skip_list/benchmarks", "" )
{
    std::vector<Benchmark> benchmarks;

    fprintf(stderr, "\nTesting");                       Progress();
    benchmarks.push_back(InsertRandomOrderedData());    Progress();
    benchmarks.push_back(InsertOrderedData());          Progress();
    benchmarks.push_back(InsertReverseOrderedData());   Progress();
    benchmarks.push_back(IterateForwards());            Progress();
    benchmarks.push_back(IterateBackwards());           Progress();
    benchmarks.push_back(Find());                       Progress();
    benchmarks.push_back(Allocation());                 Progress();
    
    fprintf(stderr, "\n\n");
    fprintf(stderr, "+===============================+===========+========+========+========+\n");
    fprintf(stderr, "|                    test title | skip_list |    set | vector |   list |\n");
    fprintf(stderr, "+-------------------------------+-----------+--------+--------+--------+\n");

    for (size_t n = 0; n < benchmarks.size(); ++n)
    {
        Benchmark &b = benchmarks[n];
        fprintf(stderr, "|%30s | %9ld | %6ld | %6ld | %6ld |\n",
                b.name.c_str(),
                b.ms_skip_list,
                b.ms_set, b.ms_vector, b.ms_list);
        int set_pc    = int(b.ms_skip_list * 100 / b.ms_set);
        int list_pc   = int(b.ms_skip_list * 100 / b.ms_list);
        int vector_pc = int(b.ms_skip_list * 100 / b.ms_vector);
        fprintf(stderr, "|%30s | %9s | %5d%% | %5d%% | %5d%% |\n",
                " ",
                " ",
                set_pc, vector_pc, list_pc);
    }
    fprintf(stderr, "+===============================+===========+========+========+========+\n");
    fprintf(stderr, "\n");
}
