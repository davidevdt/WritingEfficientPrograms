#include <benchmark/benchmark.h>

void BM_add_multiply(benchmark::State& state) {
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
        for (size_t i = 0; i < N; ++i) {
            a1 += p1[i] + p2[i]; 
        }
        benchmark::DoNotOptimize(a1); 
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations()); 
}

void BM_branch_predicted(benchmark::State& state) {
    srand(1); 
    const unsigned int N = state.range(0); 
    std::vector<unsigned long> v1(N), v2(N); 
    std::vector<int> c1(N); 
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand(); 
        v2[i] = rand(); 
        c1[i] = rand() >= 0; 

        // This would also lead to good branch prediction: 
        // if (i == 0) c1[i] = rand() > 0; 
        // else c1[i] = !c1[i-1]; 
    }
    unsigned long* p1 = v1.data(); 
    unsigned long* p2 = v2.data(); 
    int* b1 = c1.data(); 
    for (auto _: state) {
        unsigned long a1 = 0, a2 = 0; 
        for (size_t i = 0; i < N; ++i) {
            if (b1[i]) {
                a1 += p1[i]; 
            } else {
                a1 *= p2[i]; 
            }
        }
        benchmark::DoNotOptimize(a1); 
        benchmark::DoNotOptimize(a2); 
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations()); 
}

void BM_branch_not_predicted(benchmark::State& state) {
    srand(1); 
    const unsigned int N = state.range(0); 
    std::vector<unsigned long> v1(N), v2(N); 
    std::vector<int> c1(N); 
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand(); 
        v2[i] = rand(); 
        c1[i] = rand() & 1; 
    }
    unsigned long* p1 = v1.data(); 
    unsigned long* p2 = v2.data(); 
    int* b1 = c1.data(); 
    for (auto _: state) {
        unsigned long a1 = 0, a2 = 0; 
        for (size_t i = 0; i < N; ++i) {
            if (b1[i]) {
                a1 += p1[i]; 
            } else {
                a1 *= p2[i]; 
            }
        }
        benchmark::DoNotOptimize(a1); 
        benchmark::DoNotOptimize(a2); 
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations()); 
}

void BM_false_branch(benchmark::State& state) {
    srand(1); 
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    std::vector<int> c1(N), c2(N);
        for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
        c1[i] = rand() & 0x1;
        c2[i] = !c1[i];
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    int* b1 = c1.data();
    int* b2 = c2.data();
    for (auto _: state) {
        unsigned long a1 = 0, a2 = 0; 
        for (size_t i = 0; i < N; ++i) {
            if (b1[i] || b2[i]) {
                a1 += p1[i]; 
            } else {
                a1 *= p2[i]; 
            }
        }
        benchmark::DoNotOptimize(a1); 
        benchmark::DoNotOptimize(a2); 
        benchmark::ClobberMemory(); 
    }
    state.SetItemsProcessed(N*state.iterations());
    // state.SetBytesProcessed(N*sizeof(unsigned long)*state.iterations());
}

void BM_false_branch_temp(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    std::vector<int> c1(N), c2(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
        c1[i] = rand() & 0x1;
        c2[i] = !c1[i];
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    int* b1 = c1.data();
    int* b2 = c2.data();
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        for (size_t i = 0; i < N; ++i) {
            const bool c = b1[i] || b2[i];
            if (c) {
                a1 += p1[i];
            } else {
                a1 *= p2[i];
            }
        }
        benchmark::DoNotOptimize(a1);
        benchmark::DoNotOptimize(a2);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
}

void BM_false_branch_vtemp(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    std::vector<int> c1(N), c2(N), c3(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
        c1[i] = rand() & 0x1;
        c2[i] = !c1[i];
        c3[i] = c1[i] || c2[i];
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    int* b3 = c3.data();
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        for (size_t i = 0; i < N; ++i) {
            if (b3[i]) {
                a1 += p1[i];
            } else {
                a1 *= p2[i];
            }
        }
        benchmark::DoNotOptimize(a1);
        benchmark::DoNotOptimize(a2);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
}

void BM_false_branch_sum(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    std::vector<int> c1(N), c2(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
        c1[i] = rand() & 0x1;
        c2[i] = !c1[i];
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    int* b1 = c1.data();
    int* b2 = c2.data();
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        for (size_t i = 0; i < N; ++i) {
            if (b1[i] + b2[i]) {
                a1 += p1[i];
            } else {
                a1 *= p2[i];
            }
        }
        benchmark::DoNotOptimize(a1);
        benchmark::DoNotOptimize(a2);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
}

void BM_false_branch_bitwise(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    std::vector<int> c1(N), c2(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
        c1[i] = rand() & 0x1;
        c2[i] = !c1[i];
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    int* b1 = c1.data();
    int* b2 = c2.data();
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        for (size_t i = 0; i < N; ++i) {
            if (b1[i] | b2[i]) {
                a1 += p1[i];
            } else {
                a1 *= p2[i];
            }
        }
        benchmark::DoNotOptimize(a1);
        benchmark::DoNotOptimize(a2);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
}

void BM_add_multiply_unrolled_by_2(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        for (size_t i = 0; i < N; i += 2) {
            a1 += p1[i     ] * p2[i     ]
               +  p1[i +  1] * p2[i +  1]; 
        }
        benchmark::DoNotOptimize(a1);
        benchmark::DoNotOptimize(a2);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
}

void BM_branched(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    std::vector<int> c1(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
        c1[i] = rand() & 0x1;
    }
    unsigned long* p1 = v1.data();
    int* b1 = c1.data();
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        for (size_t i = 0; i < N; ++i) {
            (b1[i] ? a1 : a2) += p1[i];
        }
        benchmark::DoNotOptimize(a1);
        benchmark::DoNotOptimize(a2);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
}

void BM_branchless(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    std::vector<int> c1(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
        c1[i] = rand() & 0x1;
    }
    unsigned long* p1 = v1.data();
    int* b1 = c1.data();
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        unsigned long* a[2] = { &a2, &a1 };
        for (size_t i = 0; i < N; ++i) {
            a[b1[i]] += p1[i];
        }
        benchmark::DoNotOptimize(a1);
        benchmark::DoNotOptimize(a2);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
}

void BM_branched1(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    std::vector<int> c1(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
        c1[i] = rand() & 0x1;
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    int* b1 = c1.data();
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        for (size_t i = 0; i < N; ++i) {
            if (b1[i]) {
                a1 += p1[i];
            } else {
                a2 += p2[i];
            }
        }
        benchmark::DoNotOptimize(a1);
        benchmark::DoNotOptimize(a2);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
}

void BM_branchless1(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    std::vector<int> c1(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
        c1[i] = rand() & 0x1;
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    int* b1 = c1.data();
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        for (size_t i = 0; i < N; ++i) {
            unsigned long s1[2] = {     0, p1[i] };
            unsigned long s2[2] = { p2[i],     0 };
            a1 += s1[b1[i]];
            a2 += s2[b1[i]];
        }
        benchmark::DoNotOptimize(a1);
        benchmark::DoNotOptimize(a2);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
}

void BM_branched2(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    std::vector<int> c1(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
        c1[i] = rand() & 0x1;
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    int* b1 = c1.data();
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        for (size_t i = 0; i < N; ++i) {
            if (b1[i]) {
                a1 += p1[i] - p2[i];
            } else {
                a2 += p1[i] * p2[i];
            }
        }
        benchmark::DoNotOptimize(a1);
        benchmark::DoNotOptimize(a2);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
}

void BM_branched2_predicted(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    std::vector<int> c1(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
        c1[i] = rand() > 0;
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    int* b1 = c1.data();
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        for (size_t i = 0; i < N; ++i) {
            if (b1[i]) {
                a1 += p1[i] - p2[i];
            } else {
                a2 += p1[i] * p2[i];
            }
        }
        benchmark::DoNotOptimize(a1);
        benchmark::DoNotOptimize(a2);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
}

void BM_branchless2(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    std::vector<int> c1(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
        c1[i] = rand() & 0x1;
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    int* b1 = c1.data();
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        for (size_t i = 0; i < N; ++i) {
            unsigned long s1[2] = {             0, p1[i] - p2[i] };
            unsigned long s2[2] = { p1[i] * p2[i],             0 };
            a1 += s1[b1[i]];
            a2 += s2[b1[i]];
        }
        benchmark::DoNotOptimize(a1);
        benchmark::DoNotOptimize(a2);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
}

void BM_branchless2a(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    std::vector<int> c1(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
        c1[i] = rand() & 0x1;
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    int* b1 = c1.data();
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        unsigned long* a[2] = { &a2, &a1 };
        for (size_t i = 0; i < N; ++i) {
            unsigned long s[2] = { p1[i] * p2[i], p1[i] - p2[i] };
            a[b1[i]] += s[b1[i]];
        }
        benchmark::DoNotOptimize(a1);
        benchmark::DoNotOptimize(a2);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
}

extern unsigned long f1(unsigned long p1, unsigned long p2);
extern unsigned long f2(unsigned long p1, unsigned long p2);
void BM_branched_code(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    std::vector<int> c1(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
        c1[i] = rand() & 0x1;
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    int* b1 = c1.data();
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        for (size_t i = 0; i < N; ++i) {
            if (b1[i]) {
                a1 += f1(p1[i], p2[i]);
            } else {
                a1 += f2(p1[i], p2[i]);
            }
        }
        benchmark::DoNotOptimize(a1);
        benchmark::DoNotOptimize(a2);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
}

void BM_branchless_code(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    std::vector<int> c1(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
        c1[i] = rand() & 0x1;
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    int* b1 = c1.data();
    decltype(f1)* f[] = { f1, f2 };
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        for (size_t i = 0; i < N; ++i) {
            a1 += f[b1[i]](p1[i], p2[i]);
        }
        benchmark::DoNotOptimize(a1);
        benchmark::DoNotOptimize(a2);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
}

BENCHMARK(BM_add_multiply)->Arg(1<<22);
BENCHMARK(BM_branch_predicted)->Arg(1<<22); 
BENCHMARK(BM_branch_not_predicted)->Arg(1<<22); 
BENCHMARK(BM_false_branch)->Arg(1<<22); 
BENCHMARK(BM_false_branch_temp)->Arg(1<<22); 
BENCHMARK(BM_false_branch_vtemp)->Arg(1<<22); 
BENCHMARK(BM_false_branch_sum)->Arg(1<<22); 
BENCHMARK(BM_false_branch_bitwise)->Arg(1<<22); 
BENCHMARK(BM_add_multiply_unrolled_by_2)->Arg(1<<22); 
BENCHMARK(BM_branched)->Arg(1<<22); 
BENCHMARK(BM_branchless)->Arg(1<<22); 
BENCHMARK(BM_branched1)->Arg(1<<22); 
BENCHMARK(BM_branchless1)->Arg(1<<22); 
BENCHMARK(BM_branched2)->Arg(1<<22); 
BENCHMARK(BM_branched2_predicted)->Arg(1<<22); 
BENCHMARK(BM_branchless2)->Arg(1<<22); 
BENCHMARK(BM_branchless2a)->Arg(1<<22); 
BENCHMARK(BM_branched_code)->Arg(1<<22); 
BENCHMARK(BM_branchless_code)->Arg(1<<22); 
BENCHMARK_MAIN(); 

// g++ branching.C branch_functions.C -lbenchmark -mavx2 -o branching
// ./branching
