#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

using std::chrono::duration_cast; 
using std::chrono::milliseconds; 
using std::chrono::system_clock; 
using std::cout; 
using std::endl; 
using std::minstd_rand; 
using std::unique_ptr; 
using std::vector;

// Check runtime and profile 

// bool compare(const char* s1, const char* s2, unsigned int l);
bool compare(const char* s1, const char* s2);

int main() {
    
    // Generate random sequence 
    constexpr unsigned int L = 1 << 18, N = 1 << 14; 
    unique_ptr<char[]> s(new char[L]);
    vector<const char*> vs(N);
    minstd_rand rgen;
    // using rand_t = minstd_rand::result_type;
    ::memset(s.get(), 'a', L * sizeof(char)); // 'aaaaaa...' L times 
    for (unsigned int i = 0; i < L / 1024; ++i) {
        s[rgen() % (L - 1)] = 'a' + (rgen() % ('z' - 'a' + 1)); 
    }
    s[L - 1] = 0; 
    for (unsigned int i = 0; i < N; ++i) {
        vs[i] = &s[rgen() % (L - 1)]; 
    }
    // cout << "s=" << s.get() << endl;
    // cout << "L=" << L << ", N=" << N << endl; 
    // for (unsigned int i = 0; i < N; ++i) cout << "vs[" << i << "]=" << vs[i] << endl;

    // Sort string 
    size_t count = 0; 
    system_clock::time_point t1 = system_clock::now(); 
    std::sort(vs.begin(), vs.end(), [&](const char* a, const char* b) {
        ++count; 
        return compare(a, b); //, L); 
    }); 

    system_clock::time_point t2 = system_clock::now(); 
    cout << "Sort time: " << 
        duration_cast<milliseconds>(t2-t1).count() << 
            "ms (" << count << " comparisons)" << endl;     

}

// Compile & run (g++)
// g++ compare.C profiling.C -o profiling && ./profiling

// Profiling with google perf 
// g++ compare.C profiling.C -lprofiler -o profiling 
// CPUPROFILE=prof.data ./profiling
// CPUPROFILE=prof.data CPUPROFILE_FREQUENCY=1000 ./profiling
// pprof --text ./profiling prof.data
// or 
// pprof ./profiling prof.data 
// text --lines 

// Profiling with perf: 
// $perf stat ./profiling 
// $perf list 
// $perf stat -e cycles,instructions,branches,branch-misses,cache-references,cache-misses ./profiling

// $ perf record ./profiling
// $perf record -c 1000 ./profiling 
// $ perf report 