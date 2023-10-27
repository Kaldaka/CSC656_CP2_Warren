//
// (C) 2022-2023, E. Wes Bethel
// benchmark-* harness for running different versions of the sum study
//    over different problem sizes
//
// usage: no command line arguments
// set problem sizes, block sizes in the code below

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>
#include <string.h>

extern void setup(int64_t N, uint64_t A[]);
extern int64_t sum(int64_t N, uint64_t A[]);

int main(int argc, char** argv)
{
    std::cout << std::fixed << std::setprecision(10);

#define MAX_PROBLEM_SIZE 1 << 28
    std::vector<int64_t> problem_sizes{ MAX_PROBLEM_SIZE >> 5, MAX_PROBLEM_SIZE >> 4, MAX_PROBLEM_SIZE >> 3, MAX_PROBLEM_SIZE >> 2, MAX_PROBLEM_SIZE >> 1, MAX_PROBLEM_SIZE};

    std::vector<uint64_t> A(MAX_PROBLEM_SIZE);
    int64_t t;

    //from https://www.amd.com/en/products/cpu/amd-epyc-7763
    double capacity = 204.8e9; // Peak memory bandwidth in bytes/sec.

    for (int64_t n : problem_sizes)
    {
        std::cout << "Working on problem size N=" << n << std::endl;
        setup(n, &A[0]);

        auto start = std::chrono::high_resolution_clock::now();

        t = sum(n, &A[0]);

        auto end = std::chrono::high_resolution_clock::now();

        double duration = std::chrono::duration<double>(end - start).count();

        double mflops = (n/1e6) / duration;

        double bytes = n * sizeof(uint64_t);

        double memoryBandwidthUtilized = ((((bytes / 1e9) / duration) / capacity) * 100) * 1e9; // % of memory bandwidth utilized

        double accesses = n;

        double avgMemoryLatency = (duration / accesses) * 1e9;

        std::cout << "Time elapsed: " << duration << " s" << std::endl;
        std::cout << "MFLOP/s: " << mflops << std::endl;
        std::cout << "% Memory bandwidth utilized: " << memoryBandwidthUtilized << " bytes/ns" << std::endl;
        std::cout << "Avg memory latency: " << avgMemoryLatency << " ns/access" << std::endl;
    }

    return 0;
}
// EOF