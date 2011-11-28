#g++ test.cpp && ./a.out; rm a.out
g++ benchmark.cpp -I. -DBENCHMARK_WITH_MAIN && ./a.out; rm a.out
