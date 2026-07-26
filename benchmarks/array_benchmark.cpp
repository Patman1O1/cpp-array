// ISO C++ Includes
#include <algorithm>
#include <array> // Needed for benchmarks
#include <random>
#include <string>
#include <utility>
#include <vector>

// ISO C Includes
#include <cstring>

// Google Benchmark Includes
#include <benchmark/benchmark.h>

// Local Includes
#include <collections/array.hpp>

#define COLLECTIONS_ARRAY_SORT_BENCHMARK(N, P)                                                        \
    BENCHMARK_TEMPLATE(bench_collections_sort, N, P)                                                  \
        ->Name(std::string("collections/sort/") + pattern_name(P) + "/" #N);                          \
    BENCHMARK_TEMPLATE(bench_std_sort, N, P)->Name(std::string("std_sort/") + pattern_name(P) + "/" #N)

#define COLLECTIONS_ARRAY_SORT_BENCHMARK_SIZE(N)                 \
    COLLECTIONS_ARRAY_SORT_BENCHMARK(N, pattern::random);        \
    COLLECTIONS_ARRAY_SORT_BENCHMARK(N, pattern::sorted);        \
    COLLECTIONS_ARRAY_SORT_BENCHMARK(N, pattern::reversed);      \
    COLLECTIONS_ARRAY_SORT_BENCHMARK(N, pattern::few_unique);    \
    BENCHMARK_TEMPLATE(bench_copy_only, N)->Name("copy_only/" #N)

#define COLLECTIONS_ARRAY_STABLE_SORT_BENCHMARK(N, P)                                                                \
    BENCHMARK_TEMPLATE(bench_collections_stable_sort, N, P)                                                          \
        ->Name(std::string("collections/stable_sort/") + pattern_name(P) + "/" #N);                                  \
    BENCHMARK_TEMPLATE(bench_std_stable_sort, N, P)->Name(std::string("std_stable_sort/") + pattern_name(P) + "/" #N)

#define COLLECTIONS_ARRAY_STABLE_SORT_BENCHMARK_SIZE(N)                        \
    COLLECTIONS_ARRAY_STABLE_SORT_BENCHMARK(N, pattern::random);     \
    COLLECTIONS_ARRAY_STABLE_SORT_BENCHMARK(N, pattern::sorted);     \
    COLLECTIONS_ARRAY_STABLE_SORT_BENCHMARK(N, pattern::reversed);   \
    COLLECTIONS_ARRAY_STABLE_SORT_BENCHMARK(N, pattern::few_unique); \
    COLLECTIONS_ARRAY_STABLE_SORT_BENCHMARK(N, pattern::sawtooth);   \
    BENCHMARK_TEMPLATE(bench_copy_only, N)->Name("copy_only/" #N)

namespace collections::array_benchmarks {
    namespace {
        // A structural (NTTP-usable) enum. This must be an enum rather than a
        // struct-with-std::string: it is passed as a non-type template parameter
        // (template<std::size_t N, pattern P>), and std::string is not a valid
        // non-type template argument.
        enum class pattern : std::uint8_t {
            random,
            sorted,
            reversed,
            few_unique,
            sawtooth,
        };

        constexpr auto pattern_name(pattern p) -> const char* {
            switch (p) {
                case pattern::random:     return "random";
                case pattern::sorted:     return "sorted";
                case pattern::reversed:   return "reversed";
                case pattern::few_unique: return "few_unique";
                case pattern::sawtooth:   return "sawtooth";
            }
            return "unknown";
        }

        inline constexpr std::size_t kPoolSize = 512; // power of two; number of distinct inputs cycled

        template<std::size_t N>
        auto make_pool(pattern p) -> std::vector<int> {
            std::mt19937_64 rng(0x9E3779B97F4A7C15ull ^ N * 0x100000001B3ull ^
                                static_cast<std::uint64_t>(p));
            std::vector<int> pool(kPoolSize * N);
            for (std::size_t b = 0; b < kPoolSize; ++b) {
                int* a = pool.data() + b * N;
                switch (p) {
                    case pattern::random:     for (std::size_t i = 0; i < N; ++i) { a[i] = static_cast<int>(rng()); } break;
                    case pattern::sorted:     for (std::size_t i = 0; i < N; ++i) { a[i] = static_cast<int>(i); } break;
                    case pattern::reversed:   for (std::size_t i = 0; i < N; ++i) { a[i] = static_cast<int>(N - 1 - i); } break;
                    case pattern::few_unique: for (std::size_t i = 0; i < N; ++i) { a[i] = static_cast<int>(rng() % 16); } break;
                    case pattern::sawtooth:   for (std::size_t i = 0; i < N; ++i) { a[i] = static_cast<int>(i % 64); } break;
                }
            }
            return pool;
        }

        template<std::size_t N>
        auto is_actually_stable(const std::vector<int>& pool) -> bool {
            array<std::pair<int, int>, N> arr{};

            for (std::size_t i = 0; i < N; ++i) {
                // key with lots of collisions; second element records original position
                arr[i] = {pool[i] % 8, static_cast<int>(i)};
            }

            std::vector<std::pair<int, int>> ref(arr.begin(), arr.end());

            // Reference: stable sort by key only, so equal keys keep their original order.
            std::stable_sort(ref.begin(), ref.end(),
                [](const std::pair<int, int>& x, const std::pair<int, int>& y) -> bool {
                    return x.first < y.first;
                });

            // Same comparison via the container under test.
            arr.stable_sort(
                [](const std::pair<int, int>& x, const std::pair<int, int>& y) -> bool {
                    return x.first < y.first;
                });

            for (std::size_t i = 0; i < N; ++i) {
                if (arr[i].first != ref[i].first || arr[i].second != ref[i].second) {
                    return false;
                }
            }
            return true;
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

    // ── Stable Sort ──────────────────────────────────────────────────────────────────────────────────────────────────
    template<std::size_t N, pattern P>
    static void bench_collections_stable_sort(benchmark::State& state) {
        const std::vector<int> pool = make_pool<N>(P);
        {
            array<int, N> probe{};
            std::memcpy(probe.data(), pool.data(), N * sizeof(int));
            probe.stable_sort();
            if (!std::is_sorted(probe.begin(), probe.end())) {
                state.SkipWithError("collections::array::stable_sort produced unsorted output");
                return;
            }
            if (!is_actually_stable<N>(pool)) {
                state.SkipWithError("collections::array::stable_sort is not stable");
                return;
            }
        }

        std::size_t idx = 0;
        for (auto _ : state) {
            array<int, N> values{};
            std::memcpy(values.data(), pool.data() + idx * N, N * sizeof(int));
            benchmark::DoNotOptimize(values.data());
            values.stable_sort();
            benchmark::DoNotOptimize(values.data());
            idx = (idx + 1) & (kPoolSize - 1);
        }
        state.SetItemsProcessed(state.iterations() * N);
    }

    // std::stable_sort over std::array<int, N> -- resolves to whichever standard library this TU is built against.
    template<std::size_t N, pattern P>
    static void bench_std_stable_sort(benchmark::State& state) {
        const std::vector<int> pool = make_pool<N>(P);

        {
            std::array<int, N> probe{};
            std::memcpy(probe.data(), pool.data(), N * sizeof(int));
            std::stable_sort(probe.begin(), probe.end());
            if (!std::is_sorted(probe.begin(), probe.end())) {
                state.SkipWithError("std::stable_sort produced unsorted output");
                return;
            }
        }

        std::size_t idx = 0;
        for (auto _ : state) {
            std::array<int, N> values{};
            std::memcpy(values.data(), pool.data() + idx * N, N * sizeof(int));
            benchmark::DoNotOptimize(values.data());
            std::stable_sort(values.begin(), values.end());
            benchmark::DoNotOptimize(values.data());
            idx = (idx + 1) & (kPoolSize - 1);
        }
        state.SetItemsProcessed(state.iterations() * N);
    }

    // ── Registration matrix ──────────────────────────────────────────────────────────────────────────────────────────
    // 8/16/32 exercise the compile-time sorting network + run check; 64/256/1024 exercise the ipnsort path.
    COLLECTIONS_ARRAY_SORT_BENCHMARK_SIZE(8);
    COLLECTIONS_ARRAY_SORT_BENCHMARK_SIZE(16);
    COLLECTIONS_ARRAY_SORT_BENCHMARK_SIZE(32);
    COLLECTIONS_ARRAY_SORT_BENCHMARK_SIZE(64);
    COLLECTIONS_ARRAY_SORT_BENCHMARK_SIZE(256);
    COLLECTIONS_ARRAY_SORT_BENCHMARK_SIZE(1024);

    // Sizes span the stack-buffer path (<= 8 KB scratch) and the heap path, and the win/loss crossover.
    COLLECTIONS_ARRAY_STABLE_SORT_BENCHMARK_SIZE(64);
    COLLECTIONS_ARRAY_STABLE_SORT_BENCHMARK_SIZE(256);
    COLLECTIONS_ARRAY_STABLE_SORT_BENCHMARK_SIZE(1024);
    COLLECTIONS_ARRAY_STABLE_SORT_BENCHMARK_SIZE(8192);
    COLLECTIONS_ARRAY_STABLE_SORT_BENCHMARK_SIZE(65536);

} // namespace collections::array_benchmarks

#undef COLLECTIONS_ARRAY_SORT_BENCHMARK
#undef COLLECTIONS_ARRAY_SORT_BENCHMARK_SIZE
#undef COLLECTIONS_ARRAY_STABLE_SORT_BENCHMARK
#undef COLLECTIONS_ARRAY_STABLE_SORT_BENCHMARK_SIZE

BENCHMARK_MAIN();