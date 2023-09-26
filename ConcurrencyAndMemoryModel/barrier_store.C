#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <atomic>
#include "benchmark/benchmark.h"

#define REPEAT2(x) x x
#define REPEAT4(x) REPEAT2(x) REPEAT2(x)
#define REPEAT8(x) REPEAT4(x) REPEAT4(x)
#define REPEAT16(x) REPEAT8(x) REPEAT8(x)
#define REPEAT32(x) REPEAT16(x) REPEAT16(x)
#define REPEAT(x) REPEAT32(x)

std::atomic<unsigned long> x(0); 

// Relaxed memory order (CPU-based order, no barriers)
void BM_relaxed(benchmark::State& state) {
    for (auto _: state) {
        REPEAT(x.store(1, std::memory_order_relaxed););
        benchmark::ClobberMemory(); 
    }
    state.SetItemsProcessed(32 * state.iterations()); 
}

// Release order (show all operations that comes before the barrier)
void BM_release(benchmark::State& state) {
    for (auto _ : state) {
        REPEAT(x.store(1, std::memory_order_release););
        benchmark::ClobberMemory(); 
    }
    state.SetItemsProcessed(32 * state.iterations()); 
}

// Acquire order (show all operations that comes after the barrier)
// void BM_acquire(benchmark::State& state) {
//     for (auto _ : state) {
//         REPEAT(x.store(1, std::memory_order_acquire););
//         benchmark::ClobberMemory(); 
//     }
//     state.SetItemsProcessed(32 * state.iterations()); 
// }

// Acquire-release (show operations before and adter the barrier)
// void BM_acq_release(benchmark::State& state) {
//     for (auto _ : state) {
//         REPEAT(x.store(1, std::memory_order_acq_rel););
//         benchmark::ClobberMemory(); 
//     }
//     state.SetItemsProcessed(32 * state.iterations()); 
// }

// Sequentially consistent (all operations are shown in the right order thread-wise)
void BM_seq_cst(benchmark::State& state) {
    for (auto _ : state) {
        REPEAT(x.store(1, std::memory_order_seq_cst););
        benchmark::ClobberMemory(); 
    }
    state.SetItemsProcessed(32 * state.iterations()); 
}

static const long numcpu = sysconf(_SC_NPROCESSORS_CONF);
#define ARG ->Threads(2) ->UseRealTime()

BENCHMARK(BM_relaxed) ARG; 
BENCHMARK(BM_release) ARG; 
// BENCHMARK(BM_acquire) ARG; 
// BENCHMARK(BM_acq_release) ARG; 
BENCHMARK(BM_seq_cst) ARG; 

BENCHMARK_MAIN(); 