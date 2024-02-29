#include <iostream>
#include <chrono>
#include "loop_unrolling_and_tilling_example.hpp"
#include "prefetching_data.hpp"
#include "cache_line_aligned.hpp"
#include "false_sharing.hpp"

int main(int argc, char** argv) {
    const int matrix_size = 100000;  // Adjust the matrix size as needed
    std::vector<int> data(matrix_size);
    std::iota(data.begin(), data.end(), 1); 


    const size_t N = 600; // Matrix size for example, 4x4
    std::vector<std::vector<int>> a(N, std::vector<int>(N));
    std::vector<std::vector<int>> b(N, std::vector<int>(N));
    std::vector<std::vector<int>> c(N, std::vector<int>(N, 0)); // Initialize with zeros

    // Initialize matrices a and b with example values
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            a[i][j] = i + j; // Example initialization, you can use any logic
            b[i][j] = i - j; // Example initialization, you can use any logic
        }
    }

    false_sharing example;

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [optimized | unoptimized]\n";
        return 1;
    }
    /*cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask); // Set the CPU affinity to CPU 0

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity");
        exit(1);
    }
    */
   
    std::string mode(argv[1]);

    if (mode == "unoptimized") {
        std::cout << "Running unoptimized version\n";
        example.run_unoptimized();
    } else if (mode == "optimized") {
        std::cout << "Running optimized version\n";
        example.run_optimized();
    } else {
        std::cerr << "Invalid argument. Use 'optimized' or 'unoptimized'.\n";
        return 1;
    }

    return 0;
}


/*
#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <thread>
#include <iostream>
#include <iomanip>
#include <random>
#include <benchmark/benchmark.h>
#include "data_locality_example.hpp"
#include "loop_unrolling_and_tilling_example.hpp"

static void DataLocality_Unoptimized(benchmark::State& state) {
    data_locality_example example(state.range(0));
    for (auto _ : state) {
        example.run_unoptimized();
    }
}

static void DataLocality_Optimized(benchmark::State& state) {
    data_locality_example example(state.range(0));
    for (auto _ : state) {
        example.run_optimized();
    }
}
static void LoopUnrolling_Unoptimized(benchmark::State& state) {
    std::vector<int> data(state.range(0));
    std::iota(data.begin(), data.end(), 1); 

    loop_unrolling_and_tiling_example example(data);
    for (auto _ : state) {
        benchmark::DoNotOptimize(example.loop_unoptimized());
    }
}

static void LoopUnrolling_Optimized(benchmark::State& state) {
    std::vector<int> data(state.range(0));
    std::iota(data.begin(), data.end(), 1); 
    loop_unrolling_and_tiling_example example(data);
    for (auto _ : state) {
        benchmark::DoNotOptimize(example.loop_optimized_unrolling());
    }
}

// Register the function as a benchmark
BENCHMARK(DataLocality_Unoptimized)->Arg(10000);
BENCHMARK(DataLocality_Optimized)->Arg(10000);

BENCHMARK(LoopUnrolling_Unoptimized)->Arg(10000);
BENCHMARK(LoopUnrolling_Optimized)->Arg(10000);

// Run the benchmark
BENCHMARK_MAIN();
*/