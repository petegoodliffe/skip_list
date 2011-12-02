echo "Running unit tests..."
g++ test.cpp -I. -I.. && ./a.out && echo "Tests passed"; rm a.out

echo "Running benchmarks..."
g++ benchmark.cpp -I.. -I. -DBENCHMARK_WITH_MAIN && ./a.out; rm a.out
