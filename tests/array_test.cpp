// ISO C++ Includes
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <expected>
#include <functional>
#include <limits>
#include <random>
#include <ranges>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// Google Test Includes
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Local Includes
#include <collections/array.hpp>

namespace collections::array_testing {
    namespace {
        // ── Concepts ────────────────────────────────────────────────────────
        template<typename T, typename... Args>
        concept brace_initializable = requires { T{std::declval<Args>()...}; };

        template<typename... Args>
        concept deducible = requires {
            array{std::declval<Args>()...};
        };
    } // namespace

    // ── Aggregate Tests ─────────────────────────────────────────────────────
    namespace aggregate_testing {
        TEST(array_aggregate, is_aggregate) {
            static_assert(std::is_aggregate_v<array<int, 3>>);
            static_assert(std::is_aggregate_v<array<std::string, 2>>);

            SUCCEED();
        }

        TEST(array_aggregate, is_brace_initializable) {
            static_assert(
                brace_initializable<array<int, 3>, int, int, int>
            );
            static_assert(
                brace_initializable<array<int, 3>, int, int>
            );
            static_assert(brace_initializable<array<int, 3>, int>);
            static_assert(brace_initializable<array<int, 3>>);
            static_assert(
                !brace_initializable<
                    array<int, 3>, int, int, int, int
                >
            );
            static_assert(
                !brace_initializable<array<int, 2>, int, std::string>
            );

            SUCCEED();
        }

        TEST(array_aggregate, no_narrowing_conversions) {
            static_assert(
                !brace_initializable<array<int, 2>, double, double>
            );
            static_assert(
                !brace_initializable<array<int, 2>, int, long long>
            );
            static_assert(
                !brace_initializable<array<char, 2>, int, int>
            );
            static_assert(
                !brace_initializable<array<double, 2>, int, int>
            );

            SUCCEED();
        }

        TEST(array_aggregate, initializes_remaining_elements) {
            constexpr array<int, 3> values = {7};
            static_assert(7 == values[0]);
            static_assert(0 == values[1]);
            static_assert(0 == values[2]);

            SUCCEED();
        }

        TEST(array_aggregate, no_implicit_conversions) {
            static_assert(!std::is_convertible_v<int, array<int, 3>>);

            SUCCEED();
        }

        TEST(array_aggregate, is_default_constructible) {
            static_assert(
                std::is_default_constructible_v<array<int, 3>>
            );

            static_assert(
                std::is_default_constructible_v<array<std::string, 3>>
            );

            static_assert(
                std::is_trivially_default_constructible_v<
                    array<int, 3>
                >
            );

            static_assert(
                !std::is_trivially_default_constructible_v<
                    array<std::string, 3>
                >
            );

            SUCCEED();
        }

        TEST(array_aggregate, is_copy_constructible) {
            static_assert(
                std::is_copy_constructible_v<array<int, 3>>
            );
            static_assert(
                std::is_copy_constructible_v<array<std::string, 2>>
            );
            static_assert(
                std::is_trivially_copy_constructible_v<array<int, 3>>
            );
            static_assert(
                !std::is_trivially_copy_constructible_v<
                    array<std::string, 2>
                >
            );

            SUCCEED();
        }

        TEST(array_aggregate, copy_construction_trivial) {
            array<int, 3> src = {1, 2, 3};
            array<int, 3> dst = src;

            dst[0] = 99;

            EXPECT_EQ(1, src[0]);
            EXPECT_EQ(99, dst[0]);
        }

        TEST(array_aggregate, copy_construction_non_trivial) {
            static constexpr char cstr[] =
                "a string long enough to defeat the small string optimization";

            array<std::string, 2> src = {std::string(cstr), "beta"};
            array<std::string, 2> dst = src;

            EXPECT_EQ(src[0], dst[0]);
            EXPECT_EQ(src[1], dst[1]);
            EXPECT_NE(src.data(), dst.data());
        }

        TEST(array_aggregate, is_move_constructible) {
            static_assert(
                std::is_move_constructible_v<array<int, 3>>
            );

            static_assert(
                std::is_move_constructible_v<array<std::string, 2>>
            );

            static_assert(
                std::is_trivially_move_constructible_v<array<int, 3>>
            );

            static_assert(
                !std::is_trivially_move_constructible_v<
                    array<std::string, 2>
                >
            );

            SUCCEED();
        }
 
        TEST(
            array_aggregate,
            move_construction_non_trivial
        ) {
            static constexpr char cstr[] =
                "a string long enough to defeat the small string optimization";

            array<std::string, 1> src = {std::string(cstr)};
            array<std::string, 1> dst = std::move(src);

            EXPECT_EQ(dst[0], cstr);
            EXPECT_NE(src[0], cstr);
        }

        TEST(array_aggregate, is_destructible) {
            static_assert(std::is_destructible_v<array<int, 3>>);
            static_assert(
                std::is_destructible_v<array<std::string, 3>>
            );
            static_assert(
                std::is_trivially_destructible_v<array<int, 3>>
            );
            static_assert(
                !std::is_trivially_destructible_v<
                    array<std::string, 3>
                >
            );

            SUCCEED();
        }

        TEST(array_aggregate, is_standard_layout) {
            static_assert(std::is_standard_layout_v<array<int, 3>>);
            static_assert(sizeof(array<int, 3>) == sizeof(int[3]));
            static_assert(alignof(array<int, 3>) == alignof(int[3]));

            SUCCEED();
        }

        TEST(array_aggregate, is_deducible) {
            array values = {1, 2, 3};
            static_assert(
                std::same_as<decltype(values), array<int, 3>>
            );

            [[maybe_unused]] array singleton = {1};
            static_assert(
                std::same_as<decltype(singleton), array<int, 1>>
            );

            static_assert(deducible<int, int, int>);
            static_assert(!deducible<int, double>);

            EXPECT_EQ(1, values[0]);
            EXPECT_EQ(2, values[1]);
            EXPECT_EQ(3, values[2]);
        }

        TEST(array_aggregate, is_structural) {
            static_assert(std::is_structural_v<array<int, 3>>);

            SUCCEED();
        }
    } // namespace aggregate_testing

    // ── Overloaded Operators Tests ──────────────────────────────────────────
    namespace overloaded_operators_testing {
        TEST(array_operators, is_copy_assignable) {
            static_assert(
                std::is_copy_assignable_v<array<int, 3>>
            );
            static_assert(
                std::is_copy_assignable_v<array<std::string, 3>>
            );
            static_assert(
                std::is_trivially_copy_assignable_v<array<int, 3>>
            );
            static_assert(
                !std::is_trivially_copy_assignable_v<
                    array<std::string, 3>
                >
            );

            SUCCEED();
        }

        TEST(array_operators, is_move_assignable) {
            static_assert(
                std::is_move_assignable_v<array<int, 3>>
            );

            static_assert(
                std::is_move_assignable_v<array<std::string, 3>>
            );

            static_assert(
                std::is_trivially_move_assignable_v<array<int, 3>>
            );

            static_assert(
                !std::is_trivially_move_assignable_v<
                    array<std::string, 3>
                >
            );

            SUCCEED();
        }

        TEST(array_operators, equality_trivial_equal) {
            constexpr array<int, 3> a = {1, 2, 3};
            constexpr array<int, 3> b = {1, 2, 3};

            static_assert(a == b);
            static_assert(!(a != b));

            EXPECT_TRUE(a == b);
            EXPECT_FALSE(a != b);
        }

        TEST(array_operators, equality_trivial_not_equal) {
            constexpr array<int, 3> a = {1, 2, 3};
            constexpr array<int, 3> b = {1, 2, 4};

            static_assert(!(a == b));
            static_assert(a != b);

            EXPECT_FALSE(a == b);
            EXPECT_TRUE(a != b);
        }

        TEST(array_operators, equality_non_trivial) {
            array<std::string, 2> a = {"hello", "world"};
            array<std::string, 2> b = {"hello", "world"};
            array<std::string, 2> c = {"hello", "there"};

            EXPECT_TRUE(a == b);
            EXPECT_FALSE(a == c);
        }

        TEST(array_operators, equality_self) {
            constexpr array<int, 3> a = {1, 2, 3};
            static_assert(a == a);

            array<std::string, 2> b = {"x", "y"};
            EXPECT_TRUE(b == b);
        }

        TEST(array_operators, equality_single_element) {
            constexpr array<int, 1> a = {42};
            constexpr array<int, 1> b = {42};
            constexpr array<int, 1> c = {0};

            static_assert(a == b);
            static_assert(a != c);
        }

        TEST(array_operators, spaceship_equal) {
            constexpr array<int, 3> a = {1, 2, 3};
            constexpr array<int, 3> b = {1, 2, 3};

            static_assert((a <=> b) == 0);
            static_assert(!(a < b));
            static_assert(!(a > b));
            static_assert(a <= b);
            static_assert(a >= b);

            SUCCEED();
        }

        TEST(array_operators, spaceship_less_than_first_element) {
            constexpr array<int, 3> a = {1, 2, 3};
            constexpr array<int, 3> b = {2, 2, 3};

            static_assert((a <=> b) < 0);
            static_assert(a < b);
            static_assert(a <= b);
            static_assert(!(a > b));

            SUCCEED();
        }

        TEST(array_operators, spaceship_less_than_last_element) {
            constexpr array<int, 3> a = {1, 2, 3};
            constexpr array<int, 3> b = {1, 2, 4};

            static_assert((a <=> b) < 0);
            static_assert(a < b);

            SUCCEED();
        }

        TEST(array_operators, spaceship_greater_than) {
            constexpr array<int, 3> a = {1, 3, 3};
            constexpr array<int, 3> b = {1, 2, 3};

            static_assert((a <=> b) > 0);
            static_assert(a > b);
            static_assert(a >= b);

            SUCCEED();
        }

        TEST(array_operators, spaceship_single_element) {
            constexpr array<int, 1> a = {1};
            constexpr array<int, 1> b = {2};

            static_assert(a < b);
            static_assert(b > a);

            SUCCEED();
        }

        TEST(array_operators, subscript_mut_returns_mut_ref) {
            static_assert(
                std::same_as<
                    decltype(std::declval<array<int, 3>&>()[0]),
                    array<int, 3>::reference
                >
            );

            static_assert(
                !std::is_const_v<
                    std::remove_reference_t<
                        decltype(std::declval<array<int, 3>&>()[0])
                    >
                >
            );

            SUCCEED();
        }

        TEST(array_operators, subscript_mut_is_noexcept) {
            static_assert(
                noexcept(std::declval<array<int, 3>&>()[0])
            );

            SUCCEED();
        }

        TEST(array_operators, subscript_mut_compile_time) {
            static_assert([] -> bool {
                array values = {1, 2, 3};
                values[0] = 10;
                return 10 == values[0] && 2 == values[1] && 3 == values[2];
            }());

            SUCCEED();
        }

        TEST(array_operators, subscript_mut_aliases_storage) {
            array values = {1, 2, 3};

            const int& first  = values[0];
            int& second = values[1];

            EXPECT_EQ(1, &second - &first);

            second = 20;
            EXPECT_EQ(20, values[1]);
        }

        TEST(array_operators, subscript_const_returns_const_ref) {
            static_assert(
                std::same_as<
                    decltype(std::declval<const array<int, 3>&>()[0]),
                    array<int, 3>::const_reference
                >
            );

            static_assert(
                std::is_const_v<
                    std::remove_reference_t<
                        decltype(
                            std::declval<const array<int, 3>&>()[0]
                        )
                    >
                >
            );

            SUCCEED();
        }

        TEST(array_operators, subscript_const_is_noexcept) {
            static_assert(
                noexcept(std::declval<const array<int, 3>&>()[0])
            );

            SUCCEED();
        }

        TEST(array_operators, subscript_const_compile_time) {
            constexpr array values = {1, 2, 3};

            static_assert(1 == values[0]);
            static_assert(2 == values[1]);
            static_assert(3 == values[2]);

            SUCCEED();
        }

        TEST(array_operators, subscript_const_aliases_storage) {
            constexpr array values = {1, 2, 3};

            const int& first  = values[0];
            const int& second = values[1];

            EXPECT_EQ(1, &second - &first);
        }

        TEST(
            array_operators,
            subscript_const_does_not_copy_non_trivial
        ) {
            const array values = {
                std::string("alpha"), std::string("beta")
            };

            EXPECT_EQ("alpha", values[0]);
            EXPECT_EQ(values.data(), &values[0]);
        }
    } // namespace overloaded_operators_testing

    // ── Method Tests ────────────────────────────────────────────────────────
    namespace methods_testing {
        TEST(array_methods, at_mut_returns_mut_ref) {
            static_assert(
                std::same_as<
                    decltype(
                        std::declval<array<int, 3>&>().at(0)
                    ),
                    array<int, 3>::reference
                >
            );

            static_assert(
                !std::is_const_v<
                    std::remove_reference_t<
                        decltype(
                            std::declval<array<int, 3>&>().at(0)
                        )
                    >
                >
            );

            SUCCEED();
        }

        TEST(array_methods, at_mut_is_not_noexcept) {
            static_assert(
                !noexcept(std::declval<array<int, 3>&>().at(0))
            );

            SUCCEED();
        }

        TEST(array_methods, at_const_returns_const_ref) {
            static_assert(
                std::same_as<
                    decltype(
                        std::declval<const array<int, 3>&>().at(0)
                    ), array<int, 3>::const_reference
                >
            );

            static_assert(
                std::is_const_v<
                    std::remove_reference_t<
                        decltype(
                            std::declval<const array<int, 3>&>().at(0)
                        )
                    >
                >
            );

            SUCCEED();
        }

        TEST(array_methods, at_const_is_not_noexcept) {
            static_assert(
                !noexcept(std::declval<const array<int, 3>&>().at(0))
            );

            SUCCEED();
        }

        TEST(array_methods, at_valid_indices) {
            array values = {10, 20, 30};

            EXPECT_EQ(10, values.at(0));
            EXPECT_EQ(20, values.at(1));
            EXPECT_EQ(30, values.at(2));
        }

        TEST(array_methods, at_invalid_indices) {
            array values = {10, 20, 30};

            EXPECT_THROW(
                static_cast<void>(values.at(
                    static_cast<std::size_t>(-1))
                ),
                std::out_of_range
            );
            EXPECT_THROW(static_cast<void>(values.at(3)), std::out_of_range);
        }

        TEST(array_methods, at_mut_allows_write) {
            array values = {10, 20, 30};

            values.at(0) = 100;
            EXPECT_EQ(100, values[0]);

            values.at(2) = 300;
            EXPECT_EQ(300, values[2]);
        }

        TEST(array_methods, at_compile_time) {
            static_assert([] -> bool {
                array values = {1, 2, 3};
                values.at(0) = 10;
                return 10 == values.at(0) && 2 == values.at(1) && 3 == values.at(2);
            }());

            SUCCEED();
        }

        TEST(array_methods, at_matches_subscript) {
            constexpr array values = {10, 20, 30};

            static_assert(values.at(0) == values[0]);
            static_assert(values.at(1) == values[1]);
            static_assert(values.at(2) == values[2]);

            SUCCEED();
        }

        TEST(array_methods, front_mut_single_element) {
            static_assert(
                std::same_as<
                    decltype(std::declval<array<int, 1>&>().front()),
                    array<int, 1>::reference
                >
            );
            static_assert(noexcept(std::declval<array<int, 1>&>().front()));

            static_assert([] -> bool {
                array values = {1};
                values.front() = 10;
                return 10 == values.front() && 10 == values[0];
            }());

            array values = {1};
            EXPECT_EQ(1, values.front());
            EXPECT_EQ(values.data(), &values.front());
            EXPECT_EQ(&values.back(), &values.front());

            values.front() = 10;
            EXPECT_EQ(10, values[0]);
        }

        TEST(array_methods, front_mut_multi_element) {
            static_assert(
                std::same_as<
                    decltype(std::declval<array<int, 3>&>().front()),
                    array<int, 3>::reference
                >
            );

            static_assert(noexcept(std::declval<array<int, 3>&>().front()));

            static_assert([] -> bool {
                array values = {1, 2, 3};
                values.front() = 10;
                return 10 == values.front() && 10 == values[0] &&
                       2 == values[1] && 3 == values[2];
            }());

            array values = {1, 2, 3};
            EXPECT_EQ(1, values.front());
            EXPECT_EQ(values.data(), &values.front());
            EXPECT_NE(&values.back(), &values.front());

            values.front() = 10;
            EXPECT_EQ(10, values[0]);
            EXPECT_EQ(2,  values[1]);
            EXPECT_EQ(3,  values[2]);
        }

        TEST(array_methods, front_const_single_element) {
            static_assert(
                std::same_as<
                    decltype(std::declval<const array<int, 1>&>().front()),
                    array<int, 1>::const_reference
                >
            );

            static_assert(
                std::is_const_v<
                    std::remove_reference_t<
                        decltype(std::declval<const array<int, 1>&>().front())
                    >
                >
            );

            static_assert(
                noexcept(std::declval<const array<int, 1>&>().front())
            );

            constexpr array values = {1};
            static_assert(1 == values.front());
            static_assert(&values[0] == &values.front());
            static_assert(values.front() == values.back());

            EXPECT_EQ(1, values.front());
            EXPECT_EQ(&values[0], &values.front());
            EXPECT_EQ(&values.back(), &values.front());
        }

        TEST(array_methods, front_const_multi_element) {
            static_assert(
                std::same_as<
                    decltype(std::declval<const array<int, 3>&>().front()),
                    array<int, 3>::const_reference>
            );
            static_assert(
                std::is_const_v<
                    std::remove_reference_t<
                        decltype(std::declval<const array<int, 3>&>().front())
                    >
                >
            );

            static_assert(
                noexcept(std::declval<const array<int, 3>&>().front())
            );

            constexpr array values = {1, 2, 3};
            static_assert(1 == values.front());
            static_assert(values.front() == values[0]);

            EXPECT_EQ(1, values.front());
            EXPECT_EQ(values.data(), &values.front());
            EXPECT_NE(&values.back(), &values.front());
        }

        TEST(array_methods, back_mut_single_element) {
            static_assert(
                std::same_as<
                    decltype(std::declval<array<int, 1>&>().back()),
                    array<int, 1>::reference
                >
            );

            static_assert(noexcept(std::declval<array<int, 1>&>().back()));

            static_assert([] -> bool {
                array values = {1};
                values.back() = 10;
                return 10 == values.back() && 10 == values.front();
            }());

            array values = {1};
            EXPECT_EQ(1, values.back());
            EXPECT_EQ(values.data(), &values.back());
            EXPECT_EQ(&values.front(), &values.back());

            values.back() = 10;
            EXPECT_EQ(10, values[0]);
        }

        TEST(array_methods, back_mut_multi_element) {
            static_assert(
                std::same_as<
                    decltype(std::declval<array<int, 3>&>().back()),
                    array<int, 3>::reference
                >
            );

            static_assert(noexcept(std::declval<array<int, 3>&>().back()));

            static_assert([] -> bool {
                array values = {1, 2, 3};
                values.back() = 30;
                return 30 == values.back() && 1 == values[0] &&
                       2 == values[1] && 30 == values[2];
            }());

            array values = {1, 2, 3};
            EXPECT_EQ(3, values.back());
            EXPECT_EQ(&values[2], &values.back());
            EXPECT_EQ(2, &values.back() - &values.front());

            values.back() = 30;
            EXPECT_EQ(1,  values[0]);
            EXPECT_EQ(2,  values[1]);
            EXPECT_EQ(30, values[2]);
        }

        TEST(array_methods, back_const_single_element) {
            static_assert(
                std::same_as<
                    decltype(std::declval<const array<int, 1>&>().back()),
                    array<int, 1>::const_reference
                >
            );

            static_assert(
                std::is_const_v<
                    std::remove_reference_t<
                        decltype(std::declval<const array<int, 1>&>().back())
                    >
                >
            );

            static_assert(noexcept(std::declval<const array<int, 1>&>().back()));

            constexpr array values = {1};
            static_assert(1 == values.back());

            EXPECT_EQ(1, values.back());
            EXPECT_EQ(values.data(), &values.back());
            EXPECT_EQ(&values.front(), &values.back());
        }

        TEST(array_methods, back_const_multi_element) {
            static_assert(
                std::same_as<
                    decltype(std::declval<const array<int, 3>&>().back()),
                    array<int, 3>::const_reference
                >
            );
            static_assert(
                std::is_const_v<
                    std::remove_reference_t<
                        decltype(std::declval<const array<int, 3>&>().back())
                    >
                >
            );

            static_assert(
                noexcept(std::declval<const array<int, 3>&>().back())
            );

            constexpr array values = {1, 2, 3};
            static_assert(3 == values.back());
            static_assert(values.back() == values[2]);

            EXPECT_EQ(3, values.back());
            EXPECT_EQ(&values[2], &values.back());
            EXPECT_EQ(2, &values.back() - &values.front());
        }

        TEST(array_methods, data_returns_pointer_to_first_element) {
            static_assert(
                std::same_as<
                    decltype(std::declval<array<int, 3>&>().data()),
                    array<int, 3>::pointer
                >
            );

            static_assert(
                std::same_as<
                    decltype(std::declval<const array<int, 3>&>().data()),
                    array<int, 3>::const_pointer
                >
            );

            static_assert(noexcept(std::declval<array<int, 3>&>().data()));

            static_assert(
                noexcept(std::declval<const array<int, 3>&>().data())
            );

            array values = {1, 2, 3};
            EXPECT_EQ(&values[0], values.data());
            EXPECT_EQ(&values[2], values.data() + 2);
        }

        TEST(array_methods, data_const_is_const_pointer) {
            const array values = {1, 2, 3};
            const int* p = values.data();

            EXPECT_EQ(1, p[0]);
            EXPECT_EQ(2, p[1]);
            EXPECT_EQ(3, p[2]);
        }

        TEST(array_methods, data_points_into_values_field) {
            array<int, 3> values = {1, 2, 3};
            EXPECT_EQ(values.data(), values.values_);
        }

        TEST(array_methods, size) {
            static_assert(3 == array<int, 3>{}.size());
            static_assert(1 == array<int, 1>{}.size());
            static_assert(0 == array<int, 0>{}.size());

            static_assert(
                noexcept(std::declval<const array<int, 3>&>().size())
            );

            static_assert(
                std::same_as<
                    decltype(std::declval<const array<int, 3>&>().size()),
                    array<int, 3>::size_type
                >
            );

            SUCCEED();
        }

        TEST(array_methods, max_size) {
            static_assert(3 == array<int, 3>{}.max_size());

            SUCCEED();
        }

        TEST(array_methods, empty) {
            static_assert(!array<int, 3>{}.empty());
            static_assert(!array<int, 1>{}.empty());
            static_assert(array<int, 0>{}.empty());

            SUCCEED();
        }

        TEST(array_methods, fill_trivial) {
            static_assert([] -> bool {
                array values = {1, 2, 3};
                values.fill(7);
                return 7 == values[0] && 7 == values[1] && 7 == values[2];
            }());

            array values = {1, 2, 3};
            values.fill(7);
            EXPECT_EQ(7, values[0]);
            EXPECT_EQ(7, values[1]);
            EXPECT_EQ(7, values[2]);

            const int* const before = values.data();
            values.fill(0);
            EXPECT_EQ(before, values.data());
            EXPECT_EQ(0, values[0]);
            EXPECT_EQ(0, values[1]);
            EXPECT_EQ(0, values[2]);
        }

        TEST(array_methods, fill_non_trivial) {
            static constexpr char cstr[] =
                "a string long enough to defeat the small string optimization";

            array<std::string, 3> values;
            values.fill(cstr);

            EXPECT_EQ(cstr, values[0]);
            EXPECT_EQ(cstr, values[1]);
            EXPECT_EQ(cstr, values[2]);
        }

        TEST(array_methods, fill_noexcept) {
            static_assert(noexcept(std::declval<array<int, 3>&>().fill(0)));
            static_assert(
                !noexcept(std::declval<array<std::string, 2>&>().fill(
                    std::declval<const std::string&>())
                )
            );

            SUCCEED();
        }

        TEST(array_methods, fill_single_element) {
            array<int, 1> values = {0};
            values.fill(42);
            EXPECT_EQ(42, values[0]);
        }

        TEST(array_methods, swap_trivial) {
            static_assert([] -> bool {
                array first  = {1, 2, 3};
                array second = {4, 5, 6};
                first.swap(second);
                return 4 == first[0] && 6 == first[2] &&
                       1 == second[0] && 3 == second[2];
            }());

            array first  = {1, 2, 3};
            array second = {4, 5, 6};

            const int* const first_data  = first.data();
            const int* const second_data = second.data();

            first.swap(second);

            EXPECT_EQ(4, first[0]);
            EXPECT_EQ(5, first[1]);
            EXPECT_EQ(6, first[2]);
            EXPECT_EQ(1, second[0]);
            EXPECT_EQ(2, second[1]);
            EXPECT_EQ(3, second[2]);
            EXPECT_EQ(first_data,  first.data());
            EXPECT_EQ(second_data, second.data());
        }

        TEST(array_methods, swap_non_trivial) {
            static constexpr char long_a[] =
                "a string long enough to defeat the small string optimization";
            static constexpr char long_b[] =
                "another string long enough to defeat the small string optimization";

            array<std::string, 2> first  = {long_a, "short"};
            array<std::string, 2> second = {long_b, "tiny"};

            first.swap(second);

            EXPECT_EQ(long_b,  first[0]);
            EXPECT_EQ("tiny",  first[1]);
            EXPECT_EQ(long_a,  second[0]);
            EXPECT_EQ("short", second[1]);
        }

        TEST(array_methods, swap_noexcept_follows_element_swap) {
            static_assert(
                noexcept(std::declval<array<int, 3>&>().swap(
                    std::declval<array<int, 3>&>())
                )
            );

            static_assert(std::is_nothrow_swappable_v<std::string>);
            static_assert(
                noexcept(std::declval<array<std::string, 2>&>().swap(
                    std::declval<array<std::string, 2>&>())
                )
            );

            SUCCEED();
        }

        TEST(array_methods, sort_no_arg_ascending) {
            array values = {3, 1, 4, 1, 5, 9, 2, 6};
            values.sort();
            EXPECT_TRUE(std::is_sorted(values.begin(), values.end()));
        }

        TEST(array_methods, sort_no_arg_already_sorted) {
            array values = {1, 2, 3, 4, 5};
            values.sort();
            EXPECT_TRUE(std::is_sorted(values.begin(), values.end()));
        }

        TEST(array_methods, sort_no_arg_reverse_sorted) {
            array values = {5, 4, 3, 2, 1};
            values.sort();
            EXPECT_TRUE(std::is_sorted(values.begin(), values.end()));
        }

        TEST(array_methods, sort_no_arg_single_element) {
            array<int, 1> values = {42};
            values.sort();
            EXPECT_EQ(42, values[0]);
        }

        TEST(array_methods, sort_no_arg_preserves_all_values) {
            array values = {3, 1, 4, 1, 5, 9, 2, 6};
            const auto before = values;
            values.sort();

            array sorted_before = before;
            sorted_before.sort();
            EXPECT_EQ(sorted_before, values);
        }

        TEST(array_methods, sort_range_overload_partial) {
            array values = {5, 3, 1, 4, 2};
            values.sort(values.begin() + 1, values.begin() + 4);

            EXPECT_EQ(5, values[0]);
            EXPECT_TRUE(
                std::is_sorted(values.begin() + 1, values.begin() + 4)
            );
            EXPECT_EQ(2, values[4]);
        }

        TEST(array_methods, sort_range_overload_empty_range_no_op) {
            array values = {3, 1, 2};
            const auto before = values;
            values.sort(values.begin(), values.begin());
            EXPECT_EQ(before, values);
        }

        TEST(array_methods, sort_range_overload_full_range) {
            array values = {3, 1, 2};
            values.sort(values.begin(), values.end());
            EXPECT_TRUE(std::is_sorted(values.begin(), values.end()));
        }

        TEST(array_methods, sort_predicate_overload_descending) {
            array values = {3, 1, 4, 1, 5, 9, 2, 6};
            values.sort(values.begin(), values.end(), std::greater<int>{});
            EXPECT_TRUE(
                std::is_sorted(
                    values.begin(), values.end(), std::greater<int>{}
                )
            );
        }

        TEST(array_methods, sort_predicate_overload_partial_descending) {
            array values = {5, 3, 1, 4, 2};
            values.sort(values.begin() + 1, values.begin() + 4,
                        std::greater<int>{});

            EXPECT_EQ(5, values[0]);
            EXPECT_TRUE(std::is_sorted(values.begin() + 1, values.begin() + 4,
                                       std::greater<int>{}));
            EXPECT_EQ(2, values[4]);
        }

        TEST(array_methods, sort_predicate_overload_empty_range_no_op) {
            array values = {3, 1, 2};
            const auto before = values;
            values.sort(values.begin(), values.begin(), std::less<int>{});
            EXPECT_EQ(before, values);
        }

        TEST(array_methods, sort_non_trivial_type) {
            array<std::string, 4> values = {
                "banana", "apple", "date", "cherry"
            };

            values.sort();
            EXPECT_TRUE(std::is_sorted(values.begin(), values.end()));
        }

        TEST(array_methods, stable_sort_no_arg_ascending) {
            array values = {3, 1, 4, 1, 5, 9, 2, 6};
            values.stable_sort();
            EXPECT_TRUE(std::is_sorted(values.begin(), values.end()));
        }

        TEST(
            array_methods,
            stable_sort_no_arg_preserves_relative_order_of_equals
        ) {
            struct item {
                int key;

                int order;

                auto operator==(const struct item&) const -> bool = default;

                auto operator<=>(const struct item&) const = default;
            };

            array<struct item, 6> values = {
                item{2, 0},
                item{1, 1},
                item{2, 2},
                item{1, 3},
                item{2, 4},
                item{1, 5}
            };

            values.stable_sort(values.begin(), values.end(),
                [](const struct item& a, const struct item& b) -> bool {
                    return a.key < b.key;
                }
            );

            ASSERT_EQ(1, values[0].key);
            ASSERT_EQ(1, values[1].key);
            ASSERT_EQ(1, values[2].key);
            ASSERT_EQ(2, values[3].key);
            ASSERT_EQ(2, values[4].key);
            ASSERT_EQ(2, values[5].key);

            EXPECT_EQ(1, values[0].order);
            EXPECT_EQ(3, values[1].order);
            EXPECT_EQ(5, values[2].order);
            EXPECT_EQ(0, values[3].order);
            EXPECT_EQ(2, values[4].order);
            EXPECT_EQ(4, values[5].order);
        }

        TEST(array_methods, stable_sort_range_overload_partial) {
            array values = {5, 3, 1, 4, 2};
            values.stable_sort(values.begin() + 1, values.begin() + 4);

            EXPECT_EQ(5, values[0]);
            EXPECT_TRUE(
                std::is_sorted(values.begin() + 1, values.begin() + 4)
            );
            EXPECT_EQ(2, values[4]);
        }

        TEST(array_methods, stable_sort_range_overload_empty_range_no_op) {
            array values = {3, 1, 2};
            const auto before = values;
            values.stable_sort(values.begin(), values.begin());
            EXPECT_EQ(before, values);
        }

        TEST(array_methods, stable_sort_range_overload_full_range) {
            array values = {3, 1, 2};
            values.stable_sort(values.begin(), values.end());
            EXPECT_TRUE(std::is_sorted(values.begin(), values.end()));
        }

        TEST(array_methods, stable_sort_predicate_overload_descending) {
            array values = {3, 1, 4, 1, 5, 9, 2, 6};
            values.stable_sort(
                values.begin(),
                values.end(),
                std::greater<int>{}
            );

            EXPECT_TRUE(
                std::is_sorted(
                    values.begin(),
                    values.end(),
                    std::greater<int>{}
                )
            );
        }

        TEST(array_methods, stable_sort_predicate_overload_empty_range_no_op) {
            array values = {3, 1, 2};
            const auto before = values;
            values.stable_sort(
                values.begin(),
                values.begin(),
                std::less<int>{}
            );
            EXPECT_EQ(before, values);
        }
    } // namespace methods_testing

    // ── Iterator Tests ──────────────────────────────────────────────────────
    namespace iterator_testing {
        namespace trait_testing {
            TEST(array_iterator_traits, is_contiguous_iterator) {
                static_assert(
                    std::contiguous_iterator<array<int, 3>::iterator>
                );

                SUCCEED();
            }

            TEST(array_iterator_traits, satisfies_contiguous_range) {
                static_assert(
                    std::ranges::contiguous_range<array<int, 3>>
                );

                static_assert(
                    std::ranges::sized_range<array<int, 3>>
                );

                static_assert(
                    std::same_as<
                        std::ranges::range_value_t<array<int, 3>>, int
                    >
                );

                static_assert(
                    std::same_as<
                        decltype(std::declval<array<int, 3>&>().begin()),
                        array<int, 3>::iterator
                    >
                );

                SUCCEED();
            }
        } // namespace trait_testing
        
        namespace range_testing {
            TEST(array_iterator_range, begin_and_end) {
                static_assert([] -> bool {
                    array values = {1, 2, 3};
                    *values.begin() = 10;
                    return 10 == values[0] && 
                           values.end() - values.begin() == 3;
                }());

                const array values = {1, 2, 3};

                EXPECT_EQ(values.data(), values.begin());
                EXPECT_EQ(values.data() + 3, values.end());
                EXPECT_EQ(3, values.end() - values.begin());
                EXPECT_EQ(1, *values.begin());
                EXPECT_EQ(3, *(values.end() - 1));
            }

            TEST(array_iterator_range, range_for_loop) {
                array values = {1, 2, 3};

                for (int& value : values) {
                    value *= 2;
                }

                EXPECT_EQ(2, values[0]);
                EXPECT_EQ(4, values[1]);
                EXPECT_EQ(6, values[2]);
            }

            TEST(array_iterator_range, rbegin_and_rend) {
                array values = {1, 2, 3};

                EXPECT_EQ(3, *values.rbegin());
                EXPECT_EQ(1, *(values.rend() - 1));
                EXPECT_EQ(3, values.rend() - values.rbegin());
                EXPECT_EQ(&values.back(), &*values.rbegin());
            }

            TEST(array_iterator_range, reverse_range_for_loop) {
                array values = {1, 2, 3};
                std::vector<int> reversed(values.rbegin(), values.rend());

                EXPECT_THAT(reversed, testing::ElementsAre(3, 2, 1));
            }

            TEST(array_std_algorithms, std_fill) {
                array<int, 4> values = {};
                std::fill(values.begin(), values.end(), 7);
                EXPECT_THAT(
                    (std::vector(values.begin(), values.end())),
                    testing::ElementsAre(7, 7, 7, 7)
                );
            }

            TEST(array_std_algorithms, std_find) {
                array values = {10, 20, 30};
                auto it = std::find(values.begin(), values.end(), 20);

                ASSERT_NE(it, values.end());
                EXPECT_EQ(20, *it);
                EXPECT_EQ(values.begin() + 1, it);
            }

            TEST(array_std_algorithms, std_transform) {
                array values = {1, 2, 3};
                std::transform(values.begin(), values.end(), values.begin(),
                            [](int x) { return x * x; });

                EXPECT_EQ(1, values[0]);
                EXPECT_EQ(4, values[1]);
                EXPECT_EQ(9, values[2]);
            }

            TEST(array_std_algorithms, std_sort_via_iterators) {
                array values = {3, 1, 2};
                std::sort(values.begin(), values.end());
                EXPECT_TRUE(std::is_sorted(values.begin(), values.end()));
            }

            TEST(array_std_algorithms, ranges_for_each) {
                array values = {1, 2, 3};
                int sum = 0;
                std::ranges::for_each(values, [&sum](int x) { sum += x; });
                EXPECT_EQ(6, sum);
            }

            TEST(array_std_algorithms, ranges_sort) {
                array values = {3, 1, 2};
                std::ranges::sort(values);
                EXPECT_TRUE(std::ranges::is_sorted(values));
            }

            TEST(array_std_algorithms, ranges_reverse) {
                array values = {1, 2, 3};
                std::ranges::reverse(values);
                EXPECT_EQ(3, values[0]);
                EXPECT_EQ(2, values[1]);
                EXPECT_EQ(1, values[2]);
            }
        } // namespace range_testing
    } // namespace iterator_testing

    namespace const_iterator_testing {
        namespace trait_testing {
            TEST(array_iterator_traits, is_contiguous_iterator) {
                static_assert(
                    std::contiguous_iterator<array<int, 3>::iterator>
                );

                SUCCEED();
            }

            TEST(array_iterator_traits, satisfies_contiguous_range) {
                static_assert(
                    std::ranges::contiguous_range<array<int, 3>>
                );

                static_assert(
                    std::ranges::contiguous_range<const array<int, 3>>
                );

                static_assert(
                    std::ranges::sized_range<array<int, 3>>
                );

                static_assert(
                    std::same_as<
                        std::ranges::range_value_t<array<int, 3>>, int
                    >
                );

                static_assert(
                    std::same_as<
                        decltype(std::declval<array<int, 3>&>().begin()),
                        array<int, 3>::iterator
                    >
                );

                static_assert(
                    std::same_as<
                        decltype(std::declval<const array<int, 3>&>().begin()),
                        array<int, 3>::const_iterator
                    >
                );

                SUCCEED();
            }
        } // namespace trait_testing
        
        namespace range_testing {
            TEST(array_const_iterator_range, range_for_loop) {
                constexpr array values = {1, 2, 3};

                int sum = 0;
                for (const int value : values) {
                    sum += value;
                }

                EXPECT_EQ(6, sum);
            }

            TEST(array_const_iterator_range, cbegin_and_cend_are_const) {
                array values = {1, 2, 3};

                static_assert(
                    std::same_as<
                        decltype(values.cbegin()),
                        array<int, 3>::const_iterator>
                );

                static_assert(
                    std::is_const_v<
                        std::remove_reference_t<
                            decltype(*values.cbegin())
                        >
                    >
                );

                EXPECT_EQ(values.data(), values.cbegin());
                EXPECT_EQ(3, values.cend() - values.cbegin());
            }

            TEST(array_iterator_range, crbegin_and_crend) {
                constexpr array values = {1, 2, 3};

                static_assert(
                    std::same_as<
                        decltype(values.crbegin()),
                        array<int, 3>::const_reverse_iterator
                    >
                );

                static_assert(
                    std::is_const_v<
                        std::remove_reference_t<
                            decltype(*values.crbegin())
                        >
                    >
                );

                EXPECT_EQ(3, *values.crbegin());
                EXPECT_EQ(1, *(values.crend() - 1));
                EXPECT_EQ(3, values.crend() - values.crbegin());

                EXPECT_THAT(
                    (std::vector(values.crbegin(), values.crend())),
                    testing::ElementsAre(3, 2, 1)
                );
            }
        } // namespace range_testing
    } // namespace const_iterator_testing
} // namespace collections::array_testing
