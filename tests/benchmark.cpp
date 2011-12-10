//============================================================================
// benchmark.cpp
// Copyright (c) 2011 Pete Goodliffe. All rights reserved
//============================================================================

#include "skip_list.h"

#include "get_time.h"
#include "test_types.h"

#include <set>
#include <list>
#include <vector>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/identity.hpp>

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

// Defines a multi_index_container with a list-like index and an ordered index
typedef boost::multi_index_container
    <
        int,
        boost::multi_index::indexed_by
        <
            boost::multi_index::sequenced<>,                                           // list-like index, order unimportant (perhaps get rid of this)
            boost::multi_index::random_access<>,                                       // random access view, kept in ordered_non_unique sequence
            boost::multi_index::ordered_non_unique<boost::multi_index::identity<int> > // events in time order
        >
    >
    multi_index;

//============================================================================

struct Benchmark
{
    std::string name;
    long        vector;
    long        set;
    long        list;
    long        multi;
    long        skip_list;
    
    Benchmark()
        : name(), vector(-1), set(-1), list(-1), multi(-1), skip_list(-1) {}
    Benchmark(const std::string &name_)
        : name(name_), vector(-1), set(-1), list(-1), multi(-1), skip_list(-1) {}
};

long TimeExecutionOf(const boost::function<void()> &f);
void FillWithRandomData(size_t size, std::vector<int> &data);
void FillWithOrderedData(size_t size, std::vector<int> &data);
void FillWithReverseOrderedData(size_t size, std::vector<int> &data);

//============================================================================

int allocator_bytes_allocated = 0;
int allocator_objects_constructed = 0;

template <typename T = int>
struct TestingAllocator
{
    TestingAllocator() {}
    template <class OTHER>
    TestingAllocator(OTHER &other) : alloc(other.alloc) {}
    
    template <typename OTHER>
    struct rebind { typedef TestingAllocator<OTHER> other; };

    std::allocator<T> alloc;

    typedef T         value_type;
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    
    pointer allocate(size_type n, std::allocator<void>::const_pointer hint=0)
    {
        allocator_bytes_allocated += sizeof(T)*n;
        return alloc.allocate(n,hint);
    }
    void deallocate(pointer p, size_type n)
    {
        allocator_bytes_allocated -= sizeof(T)*n;
        return alloc.deallocate(p,n);
    }
    void construct(pointer p, const_reference val)
    {
        allocator_objects_constructed++;
        return alloc.construct(p, val);
    }
    void destroy(pointer p)
    {
        allocator_objects_constructed--;
        return alloc.destroy(p);
    }
    size_type max_size() const
    {
        return alloc.max_size();
    }
};

template<class T1, class T2> inline
bool operator==(const TestingAllocator<T1>&, const TestingAllocator<T2>&)
{
    return true;
}

template<class T1, class T2> inline
bool operator!=(const TestingAllocator<T1>&, const TestingAllocator<T2>&)
{
    return false;
}

//============================================================================

long TimeExecutionOf(const boost::function<void()> &f)
{
    const long start = get_time_us();
    f();
    const long end = get_time_us();
    return end-start;
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
void InsertIntoMultiIndex(const std::vector<int> *data, multi_index *container);
void InsertIntoMultiIndex(const std::vector<int> *data, multi_index *container)
{
    for (std::vector<int>::const_iterator i = data->begin(); i != data->end(); ++i)
    {
        multi_index::nth_index<0>::type::iterator inserted0 = container->get<0>().push_back(*i).first;
        container->get<1>().iterator_to(*inserted0);
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

//============================================================================

template <typename CONTAINER>
void Find(const CONTAINER *container)
{
    for (int n = 0 ; n < int(container->size()); ++n)
    {
        typename CONTAINER::const_iterator insert = container->find(n);
        //REQUIRE(std::distance(container->begin(), insert) == n);
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
        //REQUIRE(std::distance(container->begin(), insert) == n);
    }
}
void FindFromMultiIndex(multi_index *container);
void FindFromMultiIndex(multi_index *container)
{
    for (int n = 0 ; n < int(container->size()); ++n)
    {
        multi_index::nth_index<2>::type::iterator insert = container->get<2>().find(n);
        //REQUIRE(std::distance(, insert) == n);
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
    multi_index      multi;
    skip_list<int>   skip_list;

    Benchmark benchmark("insert data: "+name);    

    benchmark.set        = TimeExecutionOf(boost::bind(&InsertByValue<std::set<int> >, &data, &std_set));
    benchmark.list       = TimeExecutionOf(boost::bind(&InsertByValueAtRightPlace<std::list<int> >, &data, &std_list));
    benchmark.vector     = TimeExecutionOf(boost::bind(&InsertByValueAtRightPlace<std::vector<int> >, &data, &std_vector));
    benchmark.multi      = TimeExecutionOf(boost::bind(&InsertIntoMultiIndex, &data, &multi));
    benchmark.skip_list  = TimeExecutionOf(boost::bind(&InsertByValue<goodliffe::skip_list<int> >, &data, &skip_list));
    
    REQUIRE(std_list.size()     == data.size());
    REQUIRE(std_vector.size()   == data.size());
    REQUIRE(multi.size()        == data.size());

    // Remove duplcates from data, to allow size checks below to work correctly
    SortVectorAndRemoveDuplicates(data);
    
    REQUIRE(std_set.size()      == data.size());
    REQUIRE(skip_list.size()    == data.size());

    // Sanity test
    //REQUIRE(ContainerIsInOrder(std_set));
    //REQUIRE(ContainerIsInOrder(std_list));
    //REQUIRE(ContainerIsInOrder(std_vector));
    //REQUIRE(ContainerIsInOrder(skip_list));;
    //REQUIRE(ContainerIsInOrder(multi));

    return benchmark;
}

Benchmark InsertRandomData(unsigned size);
Benchmark InsertRandomData(unsigned size)
{
    std::vector<int> data;
    FillWithRandomData(size, data);
    return InsertData(data, "random order");
}

Benchmark InsertOrderedData(unsigned size);
Benchmark InsertOrderedData(unsigned size)
{
    std::vector<int> data;
    FillWithOrderedData(size, data);
    return InsertData(data, "ordered");
}

Benchmark InsertReverseOrderedData(unsigned size);
Benchmark InsertReverseOrderedData(unsigned size)
{
    std::vector<int> data;
    FillWithReverseOrderedData(size, data);
    return InsertData(data, "reverse");
}
Benchmark IterateForwards(unsigned size);
Benchmark IterateForwards(unsigned size)
{
    std::vector<int> data;
    FillWithRandomData(size, data);
    
    std::set<int>    std_set(data.begin(), data.end());
    std::list<int>   std_list(std_set.begin(), std_set.end());   // use set to ensure order
    std::vector<int> std_vector(std_set.begin(), std_set.end()); // use set to ensure order
    multi_index      multi(std_set.begin(), std_set.end()); // use set to ensure order;
    skip_list<int>   skip_list(data.begin(), data.end());
    
    Benchmark benchmark("iterate forwards");    
    
    benchmark.set        = TimeExecutionOf(boost::bind(&IterateForwardsThrough<std::set<int> >, &std_set));
    benchmark.list       = TimeExecutionOf(boost::bind(&IterateForwardsThrough<std::list<int> >, &std_list));
    benchmark.vector     = TimeExecutionOf(boost::bind(&IterateForwardsThrough<std::vector<int> >, &std_vector));
    benchmark.multi      = TimeExecutionOf(boost::bind(&IterateForwardsThrough<multi_index >, &multi));
    benchmark.skip_list  = TimeExecutionOf(boost::bind(&IterateForwardsThrough<goodliffe::skip_list<int> >, &skip_list));

    return benchmark;
}

Benchmark IterateBackwards(unsigned size);
Benchmark IterateBackwards(unsigned size)
{
    std::vector<int> data;
    FillWithRandomData(size, data);

    std::set<int>    std_set(data.begin(), data.end());
    std::list<int>   std_list(std_set.begin(), std_set.end());   // use set to ensure order
    std::vector<int> std_vector(std_set.begin(), std_set.end()); // use set to ensure order
    multi_index      multi(std_set.begin(), std_set.end()); // use set to ensure order;
    skip_list<int>   skip_list(data.begin(), data.end());
    
    Benchmark benchmark("iterate backwards");    
    
    benchmark.set        = TimeExecutionOf(boost::bind(&IterateBackwardsThrough<std::set<int> >, &std_set));
    benchmark.list       = TimeExecutionOf(boost::bind(&IterateBackwardsThrough<std::list<int> >, &std_list));
    benchmark.vector     = TimeExecutionOf(boost::bind(&IterateBackwardsThrough<std::vector<int> >, &std_vector));
    benchmark.multi      = TimeExecutionOf(boost::bind(&IterateBackwardsThrough<multi_index>, &multi));
    benchmark.skip_list  = TimeExecutionOf(boost::bind(&IterateBackwardsThrough<goodliffe::skip_list<int> >, &skip_list));

    return benchmark;
}

Benchmark Find(unsigned size);
Benchmark Find(unsigned size)
{
    std::vector<int> data;
    FillWithOrderedData(size, data);
    
    std::set<int>    std_set(data.begin(), data.end());
    std::list<int>   std_list(std_set.begin(), std_set.end());   // use set to ensure order
    std::vector<int> std_vector(std_set.begin(), std_set.end()); // use set to ensure order
    skip_list<int>   skip_list(data.begin(), data.end());
    multi_index      multi(std_set.begin(), std_set.end()); // use set to ensure order;

    multi.get<1>().rearrange(multi.get<2>().begin()); // keep random access in order

    Benchmark benchmark("find");    
    
    benchmark.set        = TimeExecutionOf(boost::bind(&Find<std::set<int> >, &std_set));
    benchmark.list       = TimeExecutionOf(boost::bind(&FindManually<std::list<int> >, &std_list));
    benchmark.vector     = TimeExecutionOf(boost::bind(&FindManually<std::vector<int> >, &std_vector));
    benchmark.multi      = TimeExecutionOf(boost::bind(&FindFromMultiIndex, &multi));
    benchmark.skip_list  = TimeExecutionOf(boost::bind(&Find<goodliffe::skip_list<int> >, &skip_list));
    
    return benchmark;
}

typedef boost::multi_index_container
    <
        int,
        boost::multi_index::indexed_by
        <
            boost::multi_index::sequenced<>,                                           // list-like index, order unimportant (perhaps get rid of this)
            boost::multi_index::random_access<>,                                       // random access view, kept in ordered_non_unique sequence
            boost::multi_index::ordered_non_unique<boost::multi_index::identity<int> > // events in time order
        >,
        TestingAllocator<int>
    >
    multi_index_allocator;

Benchmark Allocation(unsigned size);
Benchmark Allocation(unsigned size)
{
    std::vector<int> data;
    FillWithRandomData(size, data);
    
    Benchmark benchmark("allocations");

    {
        allocator_bytes_allocated     = 0;
        allocator_objects_constructed = 0;
        std::set<int,std::less<int>,TestingAllocator<int> > std_set(data.begin(), data.end());
        benchmark.set = allocator_bytes_allocated;
    }
    REQUIRE(allocator_bytes_allocated == 0);
#ifndef  _MSC_VER
    // Visual studio's std::set here appears to deallocate all the objects
    // through my allocator, but it does NOT allocate them with it.
    // WHAT?!!!!!
    // Every other container works fine.
    REQUIRE(allocator_objects_constructed == 0);
#endif

    {
        allocator_bytes_allocated     = 0;
        allocator_objects_constructed = 0;
        std::list<int,TestingAllocator<int> > std_list(data.begin(), data.end());
        benchmark.list = allocator_bytes_allocated;
    }
    REQUIRE(allocator_bytes_allocated == 0);
    
    {
        allocator_bytes_allocated     = 0;
        allocator_objects_constructed = 0;
        std::vector<int,TestingAllocator<int> > std_vector(data.begin(), data.end());
        benchmark.vector = allocator_bytes_allocated;
    }
    REQUIRE(allocator_bytes_allocated == 0);

    {
        allocator_bytes_allocated     = 0;
        allocator_objects_constructed = 0;
        multi_index_allocator multi(data.begin(), data.end());
        benchmark.multi = allocator_bytes_allocated;
    }
    REQUIRE(allocator_bytes_allocated == 0);

    {
        allocator_bytes_allocated     = 0;
        allocator_objects_constructed = 0;
        skip_list<int,std::less<int>,TestingAllocator<int> > skip_list(data.begin(), data.end());
        benchmark.skip_list = allocator_bytes_allocated;
    }
    REQUIRE(allocator_bytes_allocated == 0);

    return benchmark;
}

//============================================================================
// the mother of all comparison tests converted into a benchmark

template <typename CONTAINER>
void InsertIntInOrder(CONTAINER &container, int value)
{
    if (container.empty())
        container.push_back(value);
    else
    {
        typename CONTAINER::iterator insert = container.begin();
        while (insert != container.end() && *insert < value)
            ++insert;
        container.insert(insert, value);
    }
}

template <typename CONTAINER>
void AddInt(CONTAINER &c, int value)
    { c.insert(value); }
template<>
void AddInt<std::vector<int> >(std::vector<int> &c, int value)
    { InsertIntInOrder(c, value); }
template<>
void AddInt<std::list<int> >(std::list<int> &c, int value)
    { InsertIntInOrder(c, value); }
template<>
void AddInt<multi_index>(multi_index &c, int value)
{
    multi_index::nth_index<0>::type::iterator inserted0 = c.get<0>().push_back(value).first;
    c.get<1>().iterator_to(*inserted0);
}

template <typename CONTAINER>
void RandomUse(unsigned total_repeats, const std::vector<int> *insert, const unsigned *erase_from, const unsigned *erase_length);

template <typename CONTAINER>
void RandomUse(unsigned total_repeats, const std::vector<int> *insert, const unsigned *erase_from, const unsigned *erase_length)
{
    CONTAINER s;
    
    for (unsigned repeats = 0; repeats < total_repeats; ++repeats)
    {
        for (unsigned n = 0; n < insert[repeats].size(); ++n)
        {
            AddInt<CONTAINER>(s, insert[repeats][n]);
            //s.insert(insert[repeats][n]);
        }
        
        unsigned efrom   = erase_from[repeats];
        unsigned elength = erase_length[repeats];
        if (efrom >= s.size())        efrom   = unsigned(s.size())/2;
        if (efrom+elength > s.size()) elength = unsigned(s.size())-efrom;

        REQUIRE((efrom+elength) <= s.size());
        
        typename CONTAINER::iterator  si_from = s.begin();
        std::advance(si_from, efrom);
        
        typename CONTAINER::iterator  si_to = si_from;
        std::advance(si_to, elength);
        
        s.erase(si_from, si_to);
    }
}

Benchmark RandomUse(unsigned insert_size);
Benchmark RandomUse(unsigned insert_size)
{
    Benchmark benchmark("general use");
    
    static const unsigned repeats     = 15;
    //static const unsigned insert_size = 4000;

    std::vector<int> insert[repeats];
    unsigned         erase_from[repeats];
    unsigned         erase_length[repeats];

    for (unsigned n = 0; n < repeats; ++n)
    {
        FillWithRandomData(insert_size, insert[n]);
        
        erase_from[n]   = unsigned(rand()) % unsigned(insert_size*((n/2)+1)/3);
        erase_length[n] = unsigned(rand()) % unsigned(insert_size*((n/2)+1)/3);
    }

    benchmark.set       = TimeExecutionOf(boost::bind(&RandomUse<std::set<int> >,    repeats, insert, erase_from, erase_length));
    benchmark.vector    = TimeExecutionOf(boost::bind(&RandomUse<std::vector<int> >, repeats, insert, erase_from, erase_length));
    benchmark.list      = TimeExecutionOf(boost::bind(&RandomUse<std::list<int> >,   repeats, insert, erase_from, erase_length));
    benchmark.multi     = TimeExecutionOf(boost::bind(&RandomUse<multi_index>,       repeats, insert, erase_from, erase_length));
    benchmark.skip_list = TimeExecutionOf(boost::bind(&RandomUse<skip_list<int> >,   repeats, insert, erase_from, erase_length));

    return benchmark;
}

//============================================================================

void Progress();
void Progress() { fprintf(stderr, "."); }

#if defined(DEBUG) || defined(_DEBUG)
const char *build_type = "Debug";
#else
const char *build_type = "Release";
#endif

void RunBenchmarks(unsigned size);
void RunBenchmarks(unsigned size)
{
    std::vector<Benchmark> benchmarks;

    fprintf(stderr, "\nTesting(%d,%s)", size, build_type);  Progress();
    benchmarks.push_back(InsertRandomData(size));           Progress();
    benchmarks.push_back(InsertOrderedData(size));          Progress();
    benchmarks.push_back(InsertReverseOrderedData(size));   Progress();
    benchmarks.push_back(IterateForwards(size));            Progress();
    benchmarks.push_back(IterateBackwards(size));           Progress();
    benchmarks.push_back(Find(size));                       Progress();
    benchmarks.push_back(Allocation(size));                 Progress();
    benchmarks.push_back(RandomUse(unsigned(size*0.4)));    Progress();
    
    fprintf(stderr, "\n\n");
    fprintf(stderr, "+===============================+===========+==========+==========+==========+==========+=========+=========+=========+=========+\n");
    fprintf(stderr, "|                    test title | skip_list |      set |   vector |     list |    multi |>   set%% | vector%% |   list%% |  multi%% |\n");
    fprintf(stderr, "+-------------------------------+-----------+----------+----------+----------+----------+---------+---------+---------+---------+\n");

    for (size_t n = 0; n < benchmarks.size(); ++n)
    {
        Benchmark &b = benchmarks[n];
        int set_pc    = b.set    >0 ? int(b.skip_list * 100 / b.set)      : 0;
        int list_pc   = b.list   >0 ? int(b.skip_list * 100 / b.list)     : 0;
        int vector_pc = b.vector >0 ? int(b.skip_list * 100 / b.vector)   : 0;
        int multi_pc =  b.multi  >0 ? int(b.skip_list * 100 / b.multi)    : 0;
        fprintf(stderr, "|%30s | %9ld |%9ld |%9ld |%9ld |%9ld |>%6d%% | %6d%% | %6d%% | %6d%% |\n",
                b.name.c_str(),
                b.skip_list, b.set, b.vector, b.list, b.multi,
                set_pc, vector_pc, list_pc, multi_pc);
        //fprintf(stderr, "|%30s | %9s | %5d%% | %5d%% | %5d%% |\n",
        //        " ",
        //        " ",
        //        set_pc, vector_pc, list_pc);
    }
    fprintf(stderr, "+===============================+===========+==========+==========+==========+==========+=========+=========+=========+=========+\n");
    fprintf(stderr, "\n");
}

TEST_CASE( "skip_list/benchmarks", "" )
{
    for (unsigned size = 10; size < 10001; size *= 10)
    {
        RunBenchmarks(size);
    }
}
