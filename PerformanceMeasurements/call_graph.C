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

// Use two compare functions and check their call frequency with a call graph 

int compare(char c1, char c2) {
    if (c1 > c2) return 1; 
    if (c1 < c2) return -1; 
    return 0; 
}

bool compare1(const char* s1, const char* s2, unsigned int l) {
    if (s1 == s2) return false; 
    for (unsigned int i1 = 0, i2 = 0; i1 < l; ++i1, ++i2) {
        int res = compare(s1[i1], s2[i2]); 
        if (res != 0) return res > 0; 
    }
    return false; 
}

bool compare2(const char* s1, const char* s2, unsigned int l) {
    if (s1 == s2) return false; 
    for (unsigned int i1 = 0, i2 = 0; i1 < l; ++i1, ++i2) {
        int res = compare(s1[i1], s2[i2]); 
        if (res != 0) return res < 0; 
    }
    return false; 
}

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

    size_t count = 0;
    system_clock::time_point t1 = system_clock::now(); 
    std::sort(vs.begin(), vs.end(), [&](const char* a, const char* b) { ++count; return compare1(a, b, L); });
    system_clock::time_point t2 = system_clock::now();
    cout << "Sort time: " << duration_cast<milliseconds>(t2 - t1).count() << "ms (" << count << " comparisons)" << endl;
    std::sort(vs.begin(), vs.end(), [&](const char* a, const char* b) { ++count; return compare2(a, b, L); });
    system_clock::time_point t3 = system_clock::now();
    cout << "Second sort time: " << duration_cast<milliseconds>(t3 - t2).count() << "ms (" << count << " comparisons)" << endl;
}

// Compile & run (g++)
// g++ call_graph.C -lprofiler -o call_graph && ./call_graph
// CPUPROFILE=call_graph.data CPUPROFILE_FREQUENCY=1000 ./call_graph

// Call Graph 
// $ pprof --pdf ./call_graph call_graph.data > call_graph.pdf 
