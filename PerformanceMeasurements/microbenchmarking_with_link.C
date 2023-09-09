#include "benchmark/benchmark.h"
#include <memory> 
#include <cstring> 
using std::unique_ptr; 

extern bool compare_int(const char* s1, const char* s2); 
extern bool compare_uint(const char* s1, const char* s2); 
extern bool compare_uint_l(const char* s1, const char* s2, unsigned int l); 

void BM_loop_int(benchmark::State& state) {
    const unsigned int N = state.range(0); 
    unique_ptr<char[]> s(new char[2*N]); 
    ::memset(s.get(), 'a', 2*N*sizeof(char)); 
    s[2*N-1] = 0; 
    const char* s1 = s.get(), *s2 = s1 + N; 
    for (auto _ : state) {
        benchmark::DoNotOptimize(compare_int(s1, s2)); 
    }
    state.SetItemsProcessed(N*state.iterations()); 
}

void BM_loop_uint(benchmark::State& state) {
    const unsigned int N = state.range(0); 
    unique_ptr<char[]> s(new char[2*N]); 
    ::memset(s.get(), 'a', 2*N*sizeof(char)); 
    s[2*N-1] = 0; 
    const char* s1 = s.get(), *s2 = s1 + N; 
    for (auto _ : state) {
        benchmark::DoNotOptimize(compare_uint(s1, s2)); 
    }
    state.SetItemsProcessed(N*state.iterations()); 
}

void BM_loop_uint_l(benchmark::State& state) {
    const unsigned int N = state.range(0); 
    unique_ptr<char[]> s(new char[2*N]); 
    ::memset(s.get(), 'a', 2*N*sizeof(char)); 
    s[2*N-1] = 0; 
    const char* s1 = s.get(), *s2 = s1 + N; 
    for (auto _ : state) {
        benchmark::DoNotOptimize(compare_uint_l(s1, s2, 2*N)); 
    }
    state.SetItemsProcessed(N*state.iterations()); 
}

BENCHMARK(BM_loop_int)->Arg(1<<20); 
BENCHMARK(BM_loop_uint)->Arg(1<<20); 
BENCHMARK(BM_loop_uint_l)->Arg(1<<20); 
BENCHMARK_MAIN(); 

// Compilation: 
// g++ microbenchmarking_with_link.C compare_microbenchmarking.C -lbenchmark -lpthread -o microbenchmarking_with_link
// ./microbenchmarking_with_link