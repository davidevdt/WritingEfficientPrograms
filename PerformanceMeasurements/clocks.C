#include <chrono> 
#include <iostream> 
#include <exception> 
#include <future> 
#include <cmath> 
#include <unistd.h> // to be run in linux 
// #include <windows.h> // to be run in Windows -> modify sleep to Sleep 
using std::chrono::duration_cast; 
using std::chrono::milliseconds; 
using std::chrono::system_clock; 

double duration(timespec a, timespec b) {
    return a.tv_sec - b.tv_sec + 1e-9*(a.tv_nsec - b.tv_nsec); 
}

void TestCase0(); 
void TestCase1(); 
void TestCase2(); 
void TestCase3(); 

int main(int argc, char* argv[]) {

    uint8_t runType = 0; 

    if (argc >= 2) {
        std::string userRequest = argv[1]; 

        try {
            int tmpType = std::stoi(userRequest); 
            if (tmpType >= 0 && tmpType <= 3) {
                runType = static_cast<uint8_t>(tmpType);
            } 
        } catch (const std::invalid_argument& e) {
            
        } catch (const std::out_of_range& e){

        } catch (const std::exception& e) {

        } catch(...) {

        }
    }

    switch (runType) {
        case 0: 
            std::cout << "Testing Real-Time = CPU time = Thread Time" << std::endl;
            TestCase0();  
            break;
        case 1: 
            std::cout << "Testing Real-Time > CPU time = Thread Time" << std::endl;
            TestCase1(); 
            break; 
        case 2: 
            std::cout << "Testing Real-Time = CPU time > Thread Time" << std::endl;
            TestCase2();  
            break; 
        case 3: 
            std::cout << "Testing Real-Time ~ Thread Time < CPU Time" << std::endl;
            TestCase3(); 
            break; 
        default: 
            break; 
    }
}

void TestCase0() {
    timespec rt0, ct0, tt0; 
    clock_gettime(CLOCK_REALTIME, &rt0); 
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ct0); 
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tt0); 
    constexpr double X = 1e6; 
    double s = 0; 
    for (double x = 0; x < X; x += 0.1) s += std::sin(x); 
    timespec rt1, ct1, tt1; 
    clock_gettime(CLOCK_REALTIME, &rt1); 
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ct1); 
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tt1); 

    std::cout << "Real time: " << duration(rt1, rt0) << "s, " 
                 "CPU time: " << duration(ct1, ct0) << "s, "
                 "Thread time: " << duration(tt1, tt0) << "s" << std::endl; 
}

void TestCase1() {
    timespec rt0, ct0, tt0; 
    clock_gettime(CLOCK_REALTIME, &rt0); 
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ct0); 
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tt0); 
    sleep(1); 
    timespec rt1, ct1, tt1; 
    clock_gettime(CLOCK_REALTIME, &rt1); 
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ct1); 
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tt1); 

    std::cout << "Real time: " << duration(rt1, rt0) << "s, " 
                 "CPU time: " << duration(ct1, ct0) << "s, "
                 "Thread time: " << duration(tt1, tt0) << "s" << std::endl; 
}

void TestCase2() {
    // The overall program is doing heavy computing, 
    // but the thread that calls the timers is mostly sleeping 
    timespec rt0, ct0, tt0; 
    clock_gettime(CLOCK_REALTIME, &rt0); 
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ct0); 
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tt0); 
    constexpr double X = 1e6; 
    double s = 0; 
    auto f = std::async(std::launch::async, 
        [&] {for (double x = 0; x < X; x += 0.1) s += std::sin(x); 
        }); 
    f.wait();  
    timespec rt1, ct1, tt1; 
    clock_gettime(CLOCK_REALTIME, &rt1); 
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ct1); 
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tt1); 

    std::cout << "Real time: " << duration(rt1, rt0) << "s, " 
                 "CPU time: " << duration(ct1, ct0) << "s, "
                 "Thread time: " << duration(tt1, tt0) << "s" << std::endl; 
}

void TestCase3() {
    // Now both threads (main and thread) are doing computations 
    // So cpu time is doubled w.r.t real-time 
    timespec rt0, ct0, tt0; 
    clock_gettime(CLOCK_REALTIME, &rt0); 
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ct0); 
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tt0); 
    constexpr double X = 1e6; 
    double s1 = 0, s2 = 0; 
    auto f = std::async(std::launch::async, 
        [&] {for (double x = 0; x < X; x += 0.1) s1 += std::sin(x); 
        }); 
    for (double x = 0; x < X; x += 0.1) s2 += std::sin(x); 
    f.wait();  
    timespec rt1, ct1, tt1; 
    clock_gettime(CLOCK_REALTIME, &rt1); 
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ct1); 
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tt1); 

    std::cout << "Real time: " << duration(rt1, rt0) << "s, " 
                 "CPU time: " << duration(ct1, ct0) << "s, "
                 "Thread time: " << duration(tt1, tt0) << "s" << std::endl; 
}

// g++ clocks.C -o clocks  && ./clocks 0
// ./clocks 1 
// ./clocks 2 
// ./clocks 3 