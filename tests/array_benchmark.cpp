// ISO C++ Includes
#include <array> // For benchmarks

// Google Benchmark Includes
#include <benchmark/benchmark.h>

// Local Includes
#include <collections/array.hpp>

namespace collections::array_benchmarks {
    static void collections_array_aggregate_init(benchmark::State& state) {
        // Outside the loop: defeats constant-folding of the inputs without
        // forcing a reload every iteration.
        int x = 1;
        int y = 2;
        int z = 3;
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(y);
        benchmark::DoNotOptimize(z);

        for (auto _ : state) {
            array values = {x, y, z};
            benchmark::DoNotOptimize(values);
            benchmark::ClobberMemory();
        }
    }
    BENCHMARK(collections_array_aggregate_init);

    static void std_array_aggregate_init(benchmark::State& state) {
        int x = 1;
        int y = 2;
        int z = 3;
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(y);
        benchmark::DoNotOptimize(z);

        for (auto _ : state) {
            std::array values = {x, y, z};
            benchmark::DoNotOptimize(values);
            benchmark::ClobberMemory();
        }
    }
    BENCHMARK(std_array_aggregate_init);
} // namespace collections::array_benchmarks

BENCHMARK_MAIN();