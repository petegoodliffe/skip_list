skip_list
=========

Pete Goodliffe (pete /at/ goodliffe.net)


This is a set of STL-style C++ skip list containers. The skip list is a very useful,
reasonably fast ordered container.

The skip list provides fast searching, and good insert/erase performance.
You could consider it an interesting hybrid of a std::list and a std::set.

Unlike most skip list implementations, you can iterate bi-directionally. Unlike
traditional skip lists, I also provide additional containers providing full random
access and multiple-identical item containment.

This implementation provides a familiar C++ interface, with bidirectional iterators
and compatibility with STL algorithms. It should drop cleanly into any modern
C++ project.

Following the freaky STL container names, this container might be better named
unique_sorted_list or sorted_list, or somesuch other drivel. In the interests of
clarity, and my own sanity, I have called it skip_list.

Examples:
    skip_list<int> list;
    list.insert(1);
    list.insert(2);
    list.insert(3);
    std::copy(list.begin(), list.end(), std::ostream_iterator<int>(std::cout, "\n"));

There are several containers provided here:

* *skip_list* The basic skip-list. Provides bidirectional iteration. Only allows you
  to insert a value into the list once. The API is akin to std::list and std::set.
* *multi_slip_list* As multiset is to set, this is a skip list that allows to you insert
  the same value multiple times. Also provides bidirectional iteration. Supports the
  additional operations provides by multiset (count, lower_bound, upper_bound, equal_range)
* *random_access_skip_list* A skip list variant that provides fast random access via
  indexing (i.e. operator[]) and a full random access iterator. This provides many
  of the benefits of std::vector, but with stable items in the list, hence non-invalidating
  iterators and iterator mathematics.

The basic skip_list provides the best performance, at the cost of fewer features.
The multi_skip_list works slightly slower to provide multiple-identical-item insertion.
The random_access_skip_list uses a little more memory to support fast random-access.


Performance
-----------

In the test set I include a simple benchmarking test comparing the skip_list against
other STL data structures for working with an ordered set of data.


Compatibility
-------------

I have tested this container on:

* Mac OS using Xcode 4.2
* Windows using Visual Studio 2008
* Linux using gcc 4.4

I have not yet provided C++11 "move" or initializer_list operations.


Usage notes
-----------

The only file you really care about looking at is "skip_list.h". Everything else in
this directory is supporting gumph. To use this container in your project, just
copy over that file.

In debug builds (if you #define DEBUG) then a number of diagnostcs are enabled
in the container that may affect performance slightly.

I use the Catch unit test framework to run my tests. The single-header version of
this is included in the repository. [Catch]

To compile the benchmarks, I make use of the Boost library. I don't include Boost
[Boost] in this repository. The easiest way to use Boost here is to copy to Boost header
file directory (i.e the $BOOST_ROOT/boost directory) into this project directory.

* [Catch] https://github.com/philsquared/Catch
* [Boost] http://www.boost.org/


Documentation
-------------

I've documented parts of the source in Doxygen syntax, but I've not gone
to town with this.

The interface will be perfectly familiar to a C++ programmer.


License
-------

Feel free to incorporate this code in your own applications.

I'd appreciate hearing from you if you do so. It's nice to know that I've been helpful. Attribution is welcomed, but not required.

Copyright (c) 2010-2011 Pete Goodliffe. All rights reserved.


Release Notes
-------------

This is still a work in progress, please check back for updates.
