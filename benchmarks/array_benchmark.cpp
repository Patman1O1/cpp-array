// ISO C++ Includes
#include <algorithm> // For std::sort (needed for benchmarks)
#include <array> // For std::array (needed for benchmarks)
#include <random>
#include <string>
#include <vector>

// ISO C Includes
#include <cstring>

// Google Benchmark Includes
#include <benchmark/benchmark.h>

// Local Includes
#include <collections/array.hpp>

namespace collections::array_benchmarks {
    namespace {
        // ── Enums ────────────────────────────────────────────────────────────────────────────────────────────────────
        enum class pattern { random, sorted, reversed, few_unique };

        // ── Classes ──────────────────────────────────────────────────────────────────────────────────────────────────


        // ── Constants ────────────────────────────────────────────────────────────────────────────────────────────────
        constexpr std::size_t kPoolSize = 512; // power of two; number of distinct inputs cycled

        // ── Functions ────────────────────────────────────────────────────────────────────────────────────────────────
        constexpr auto pattern_name(const pattern pattern) -> const char* {
            switch (pattern) {
                case pattern::random: return "random";
                case pattern::sorted: return "sorted";
                case pattern::reversed: return "reversed";
                case pattern::few_unique: return "few_unique";
            }
            return "?";
        }

        template<std::size_t N>
        auto make_pool(const pattern pattern) -> std::vector<int> {
            std::mt19937_64 rng(0x9E3779B97F4A7C15ull ^ N * 0x100000001B3ull ^ static_cast<std::uint64_t>(pattern));
            std::vector<int> pool(kPoolSize * N);
            for (std::size_t b = 0; b < kPoolSize; b++) {
                int* a = pool.data() + b * N;
                switch (pattern) {
                    case pattern::random:
                        for (std::size_t i = 0; i < N; i++) {
                            a[i] = static_cast<int>(rng());
                        }
                        break;
                    case pattern::sorted:
                        for (std::size_t i = 0; i < N; i++) {
                            a[i] = static_cast<int>(i);
                        }
                        break;
                    case pattern::reversed:
                        for (std::size_t i = 0; i < N; i++) {
                            a[i] = static_cast<int>(N - i);
                        }
                        break;
                    case pattern::few_unique:
                        for (std::size_t i = 0; i < N; i++) {
                            a[i] = static_cast<int>(rng() % 4);
                        }
                        break;
                }
            }
            return pool;
        }
    } // namespace

    // ── Aggregate initialization ─────────────────────────────────────────────────────────────────────────────────────
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
            auto my_arr = std::array{1, 2, 3};
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

    // ── Sort ─────────────────────────────────────────────────────────────────────────────────────────────────
    // collections::array<int, N>::sort()
    template<std::size_t N, pattern P>
    static void bench_collections_sort(benchmark::State& state) {
        const std::vector<int> pool = make_pool<N>(P);

        // Correctness guard (untimed): a sort that does not sort must not post a fast time.
        {
            collections::array<int, N> probe{};
            std::memcpy(probe.data(), pool.data(), N * sizeof(int));
            probe.sort();
            if (!std::is_sorted(probe.begin(), probe.end())) {
                state.SkipWithError("collections::array::sort produced unsorted output");
                return;
            }
        }

        std::size_t idx = 0;
        for (auto _ : state) {
            collections::array<int, N> values{};
            std::memcpy(values.data(), pool.data() + idx * N, N * sizeof(int));
            benchmark::DoNotOptimize(values.data());
            values.sort();
            benchmark::DoNotOptimize(values.data());
            idx = (idx + 1) & (kPoolSize - 1);
        }
        state.SetItemsProcessed(state.iterations() * N);
    }

    // std::sort over std::array<int, N> -- resolves to whichever standard library this TU is built against.
    template<std::size_t N, pattern P>
    static void bench_std_sort(benchmark::State& state) {
        const std::vector<int> pool = make_pool<N>(P);

        {
            std::array<int, N> probe{};
            std::memcpy(probe.data(), pool.data(), N * sizeof(int));
            std::sort(probe.begin(), probe.end());
            if (!std::is_sorted(probe.begin(), probe.end())) {
                state.SkipWithError("std::sort produced unsorted output");
                return;
            }
        }

        std::size_t idx = 0;
        for (auto _ : state) {
            std::array<int, N> values{};
            std::memcpy(values.data(), pool.data() + idx * N, N * sizeof(int));
            benchmark::DoNotOptimize(values.data());
            std::sort(values.begin(), values.end());
            benchmark::DoNotOptimize(values.data());
            idx = (idx + 1) & (kPoolSize - 1);
        }
        state.SetItemsProcessed(state.iterations() * N);
    }

    // Refill-only baseline: what both benchmarks do EXCEPT the sort. Subtract to get absolute sort time.
    template<std::size_t N>
    static void bench_copy_only(benchmark::State& state) {
        const std::vector<int> pool = make_pool<N>(pattern::random);
        std::size_t idx = 0;
        for (auto _ : state) {
            std::array<int, N> values{};
            std::memcpy(values.data(), pool.data() + idx * N, N * sizeof(int));
            benchmark::DoNotOptimize(values.data());
            idx = (idx + 1) & (kPoolSize - 1);
        }
    }

    // ── Registration matrix ──────────────────────────────────────────────────────────────────────────────────────────
    // Build twice to compare standard libraries:
    //   clang++ -stdlib=libc++  -O3 -march=native  -> std_sort/* rows are libc++'s std::sort
    //   g++     (libstdc++)     -O3 -march=native  -> std_sort/* rows are libstdc++'s std::sort
    // collections/sort/* is stdlib-independent; compare each std_sort row to its collections counterpart in the
    // same build.

#define COLLECTIONS_BENCH_ONE(N, P)                                                                                     \
    BENCHMARK_TEMPLATE(bench_collections_sort, N, P)                                                                    \
        ->Name(std::string("collections/sort/") + pattern_name(P) + "/" #N);                                           \
    BENCHMARK_TEMPLATE(bench_std_sort, N, P)->Name(std::string("std_sort/") + pattern_name(P) + "/" #N)

#define COLLECTIONS_BENCH_SIZE(N)                                                                                       \
    COLLECTIONS_BENCH_ONE(N, pattern::random);                                                                          \
    COLLECTIONS_BENCH_ONE(N, pattern::sorted);                                                                          \
    COLLECTIONS_BENCH_ONE(N, pattern::reversed);                                                                        \
    COLLECTIONS_BENCH_ONE(N, pattern::few_unique);                                                                      \
    BENCHMARK_TEMPLATE(bench_copy_only, N)->Name("copy_only/" #N)

    // 8/16/32 exercise the compile-time sorting network + run check; 64/256/1024 exercise the ipnsort path.
    COLLECTIONS_BENCH_SIZE(8);
    COLLECTIONS_BENCH_SIZE(16);
    COLLECTIONS_BENCH_SIZE(32);
    COLLECTIONS_BENCH_SIZE(64);
    COLLECTIONS_BENCH_SIZE(256);
    COLLECTIONS_BENCH_SIZE(1024);

#undef COLLECTIONS_BENCH_SIZE
#undef COLLECTIONS_BENCH_ONE

} // namespace collections::array_benchmarks

BENCHMARK_MAIN();