#include <benchmark/benchmark.h>

void BM_one_step(benchmark::State& state) {
    srand(1); 
    const unsigned int N = state.range(0); 
    std::vector<unsigned long> v1(N), v2(N); 
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand(); 
        v2[i] = rand(); 
    }
    unsigned long* p1 = v1.data(); 
    unsigned long* p2 = v2.data(); 
    for (auto _: state) {
        unsigned long a1 = 0; 
        std::vector<unsigned long> s(N, 0), d(N, 0); 
        for (size_t i = 0; i < N; ++i) {
            // a1 += (p1[i] + p2[i]) * (p1[i] - p2[i]); 
            s[i] = (p1[i] + p2[i]); 
            d[i] = (p1[i] - p2[i]);
            a1 += s[i] * d[i];  
        }
        benchmark::DoNotOptimize(a1); 
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations()); 
}

void BM_pipelined(benchmark::State& state) {
    srand(1); 
    const unsigned int N = state.range(0); 
    std::vector<unsigned long> v1(N), v2(N); 
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand(); 
        v2[i] = rand(); 
    }
    unsigned long* p1 = v1.data(); 
    unsigned long* p2 = v2.data(); 
    for (auto _: state) {
        unsigned long a1 = 0; 
        std::vector<unsigned long> s(N, 0), d(N, 0); 
        s[0] = (p1[0] - p2[0]); 
        d[0] = (p1[0] - p2[0]); 
        for (size_t i = 1; i < N; ++i) {
            s[i] = (p1[i] + p2[i]); 
            d[i] = (p1[i] - p2[i]);
            a1 += s[i-1] * d[i-1];  
        }
        a1 += (s[N-1] * d[N-1]); 
        benchmark::DoNotOptimize(a1); 
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations()); 
}

BENCHMARK(BM_one_step)->Arg(1<<22);
BENCHMARK(BM_pipelined)->Arg(1<<22); 
BENCHMARK_MAIN(); 

// g++ pipelining.C -lbenchmark -mavx2 -o pipelining
// ./pipelining
