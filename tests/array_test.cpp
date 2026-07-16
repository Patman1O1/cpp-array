// ISO C++ Includes
#include <concepts>
#include <expected>
#include <functional>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

// Google Test Includes
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// Local Includes
#include <collections/array.hpp>

namespace collections::array_testing {
    namespace {
        // ── Aliases ──────────────────────────────────────────────────────────────────────────────────────────────────
        using ::testing::Eq;
        using ::testing::Ne;

        // An element type whose conversion from int is explicit-only, for testing `explicit(...)`.
        // ── struct explicit_from_int ─────────────────────────────────────────────────────────────────────────────────
        struct explicit_from_int {
            // ── Fields ───────────────────────────────────────────────────────────────────────────────────────────────
            int value;

            // ── Constructors ─────────────────────────────────────────────────────────────────────────────────────────
            inline constexpr explicit_from_int() noexcept : value(0) {}

            inline constexpr explicit explicit_from_int(const int value) noexcept : value(value) {}

            inline constexpr explicit_from_int(const explicit_from_int&) noexcept = default;

            inline constexpr explicit_from_int(explicit_from_int&&) noexcept = default;

            // ── Destructor ───────────────────────────────────────────────────────────────────────────────────────────
            inline constexpr ~explicit_from_int() noexcept = default;

            // ── Overloaded Operators ─────────────────────────────────────────────────────────────────────────────────
            inline constexpr explicit_from_int& operator=(const explicit_from_int&) noexcept = default;

            inline constexpr explicit_from_int& operator=(explicit_from_int&&) noexcept = default;

            inline constexpr bool operator==(const explicit_from_int&) const noexcept = default;

            inline constexpr auto operator<=>(const explicit_from_int&) const noexcept = default;
        };
    } // namespace

    // ── Constructor Tests ────────────────────────────────────────────────────────────────────────────────────────────
    TEST(array_constructors, variadic_overload_checks_arity_at_compile_time) {
        // This is the property an initializer_list constructor cannot have: `il.size()` is a runtime value.
        static_assert(std::is_constructible_v<array<int, 3>, int, int, int>);
        static_assert(std::is_constructible_v<array<int, 3>, int, int>);
        static_assert(std::is_constructible_v<array<int, 3>, int>);
        static_assert(!std::is_constructible_v<array<int, 3>, int, int, int, int>);

        // The constraint rejects arguments that cannot produce a value_type.
        static_assert(!std::is_constructible_v<array<int, 3>, int, std::string>);

        SUCCEED();
    }

    TEST(array_constructors, variadic_overload_value_initializes_the_unsupplied_tail) {
        constexpr array<int, 3> values = {7};
        static_assert(7 == values[0]);
        static_assert(0 == values[1]);
        static_assert(0 == values[2]);

        SUCCEED();
    }

    TEST(array_constructors, variadic_overload_is_explicit_only_when_an_element_conversion_is) {
        // int -> int is implicit, so the constructor is implicit and copy-list-init works.
        static_assert(std::is_convertible_v<int, array<int, 3>>);

        // int -> explicit_from_int is explicit-only, so the constructor becomes explicit.
        static_assert(std::is_constructible_v<array<explicit_from_int, 3>, int, int, int>);
        static_assert(!std::is_convertible_v<int, array<explicit_from_int, 3>>);

        SUCCEED();
    }

    TEST(array_constructors, variadic_overload_computes_noexcept_from_the_argument_value_categories) {
        static_assert(std::is_nothrow_constructible_v<array<int, 3>, int, int, int>);

        // Args deduces `std::string` for rvalues -> move -> nothrow.
        static_assert(std::is_nothrow_constructible_v<array<std::string, 2>, std::string, std::string>);

        // Args deduces `std::string&` for lvalues -> copy -> may throw.
        static_assert(!std::is_nothrow_constructible_v<array<std::string, 2>, std::string&, std::string&>);

        // Constructing a string from a pointer allocates.
        static_assert(!std::is_nothrow_constructible_v<array<std::string, 2>, const char*, const char*>);

        SUCCEED();
    }

    TEST(array_constructors, variadic_overload_moves_rvalue_elements) {
        std::string first = "a string long enough to defeat the small string optimization";
        std::string second = "another string long enough to defeat the small string optimization";

        const array<std::string, 2> values = {std::move(first), std::move(second)};

        EXPECT_THAT(values[0], Eq("a string long enough to defeat the small string optimization"));
        EXPECT_THAT(values[1], Eq("another string long enough to defeat the small string optimization"));

        // Moved-from is unspecified-but-valid; only the fact that a move happened is checkable.
        EXPECT_THAT(first, Ne("a string long enough to defeat the small string optimization"));
        EXPECT_THAT(second, Ne("another string long enough to defeat the small string optimization"));
    }

    TEST(array_constructors, copy_from_a_non_const_lvalue_selects_the_copy_constructor) {
        array<int, 3> source = {1, 2, 3};

        // Without the self-exclusion clause in the requires-clause, the variadic constructor deduces
        // `Args = array<int, 3>&`, wins on exact match, and this line fails to compile on `static_cast<int>(source)`.
        array<int, 3> copy = source;

        EXPECT_THAT(copy[0], Eq(1));
        EXPECT_THAT(copy[1], Eq(2));
        EXPECT_THAT(copy[2], Eq(3));
    }

    TEST(array_constructors, special_members_stay_trivial) {
        // This is the whole payoff of defaulting them: trivially copyable types get passed in registers
        // and memcpy'd by every stdlib algorithm.
        static_assert(std::is_trivially_default_constructible_v<array<int, 3>>);
        static_assert(std::is_trivially_copy_constructible_v<array<int, 3>>);
        static_assert(std::is_trivially_move_constructible_v<array<int, 3>>);
        static_assert(std::is_trivially_copy_assignable_v<array<int, 3>>);
        static_assert(std::is_trivially_move_assignable_v<array<int, 3>>);
        static_assert(std::is_trivially_destructible_v<array<int, 3>>);
        static_assert(std::is_trivially_copyable_v<array<int, 3>>);

        // A non-trivial value_type must propagate, not be papered over.
        static_assert(!std::is_trivially_copyable_v<array<std::string, 2>>);
        static_assert(std::is_copy_constructible_v<array<std::string, 2>>);
        static_assert(std::is_move_constructible_v<array<std::string, 2>>);
        static_assert(std::is_copy_assignable_v<array<std::string, 2>>);
        static_assert(std::is_move_assignable_v<array<std::string, 2>>);

        SUCCEED();
    }

    TEST(array_constructors, copy_assignment_is_not_deleted) {
        // Declaring a move constructor without declaring the assignment operators would delete this.
        array<int, 3> source = {1, 2, 3};
        array<int, 3> target = {};

        target = source;

        EXPECT_THAT(target[0], Eq(1));
        EXPECT_THAT(target[1], Eq(2));
        EXPECT_THAT(target[2], Eq(3));
    }

    // ── Overloaded Operators Tests ───────────────────────────────────────────────────────────────────────────────────
    TEST(array_operators, random_access_mut_overload_returns_mut_ref) {
        static_assert(std::same_as<decltype(std::declval<array<int, 3>&>()[0]), array<int, 3>::reference>);
        static_assert(!std::is_const_v<std::remove_reference_t<decltype(std::declval<array<int, 3>&>()[0])>>);

        SUCCEED();
    }

    TEST(array_operators, random_access_mut_overload_is_noexcept) {
        static_assert(noexcept(std::declval<array<int, 3>&>()[0]));

        SUCCEED();
    }

    TEST(array_operators, random_access_mut_overload_is_usable_in_constant_expressions) {
        // A `constexpr` object is const, so it can never select the mutable overload.
        // The mutation has to happen inside the constant evaluation.
        static_assert([] {
            array<int, 3> values = {1, 2, 3};
            values[0] = 10;
            return 10 == values[0] && 2 == values[1] && 3 == values[2];
        }());

        SUCCEED();
    }

    TEST(array_operators, random_access_mut_overload_aliases_storage) {
        array<int, 3> values = {1, 2, 3};

        const int& first = values[0];
        int& second = values[1];

        // Contiguous adjacent elements: this is what pins down "reference" rather than "value".
        EXPECT_THAT(&second - &first, Eq(1));

        second = 20;
        EXPECT_THAT(values[1], Eq(20));
    }

    TEST(array_operators, random_access_const_overload_returns_const_ref) {
        static_assert(std::same_as<decltype(std::declval<const array<int, 3>&>()[0]), array<int, 3>::const_reference>);
        static_assert(std::is_const_v<std::remove_reference_t<decltype(std::declval<const array<int, 3>&>()[0])>>);

        SUCCEED();
    }

    TEST(array_operators, random_access_const_overload_is_noexcept) {
        static_assert(noexcept(std::declval<const array<int, 3>&>()[0]));

        SUCCEED();
    }

    TEST(array_operators, random_access_const_overload_is_usable_in_constant_expressions) {
        constexpr array<int, 3> values = {1, 2, 3};
        static_assert(1 == values[0]);
        static_assert(2 == values[1]);
        static_assert(3 == values[2]);

        SUCCEED();
    }

    TEST(array_operators, random_access_const_overload_aliases_storage) {
        constexpr array<int, 3> values = {1, 2, 3};

        const int& first = values[0];
        const int& second = values[1];

        EXPECT_THAT(&second - &first, Eq(1));
    }

    TEST(array_operators, random_access_const_overload_does_not_copy_a_non_trivial_element) {
        const array<std::string, 2> values = {std::string("alpha"), std::string("beta")};

        EXPECT_THAT(values[0], Eq("alpha"));
        EXPECT_THAT(&values[0], Eq(&values[0]));  // stable address across calls: no temporary
    }

    // ── Method Tests ─────────────────────────────────────────────────────────────────────────────────────────────────
    TEST(array_methods, at_mut_overload_returns_mut_ref) {
        static_assert(std::same_as<decltype(std::declval<array<int, 3>&>().at(0)), array<int, 3>::reference>);
        static_assert(!std::is_const_v<std::remove_reference_t<decltype(std::declval<array<int, 3>&>().at(0))>>);

        SUCCEED();
    }

    TEST(array_methods, at_mut_overload_is_not_noexcept) {
        static_assert(!noexcept(std::declval<array<int, 3>&>().at(0)));

        SUCCEED();
    }

    TEST(array_methods, at_mut_overload_does_not_throw) {
        // Mutable object inside the constant evaluation, so the mutable overload is actually selected.
        static_assert([] {
            array<int, 3> values = {1, 2, 3};
            values.at(0) = 10;
            return 10 == values.at(0) && 2 == values.at(1) && 3 == values.at(2);
        }());

        array<int, 3> values = {1, 2, 3};
        EXPECT_NO_THROW(static_cast<void>(values.at(0)));
        EXPECT_NO_THROW(static_cast<void>(values.at(2)));

        values.at(2) = 30;
        EXPECT_THAT(values[2], Eq(30));
        EXPECT_THAT(&values.at(2), Eq(&values[2]));
    }

    TEST(array_methods, at_mut_overload_throws) {
        // Must be non-const. The original test used a `constexpr` object, which is const,
        // so it was calling the const overload and duplicating at_const_overload_throws.
        array<int, 3> values = {1, 2, 3};

        EXPECT_THROW(static_cast<void>(values.at(3)), std::out_of_range);
        EXPECT_THROW(static_cast<void>(values.at(std::numeric_limits<array<int, 3>::size_type>::max())),
                     std::out_of_range);
    }

    TEST(array_methods, at_const_overload_returns_const_ref) {
        static_assert(std::same_as<decltype(std::declval<const array<int, 3>&>().at(0)), array<int, 3>::const_reference>);
        static_assert(std::is_const_v<std::remove_reference_t<decltype(std::declval<const array<int, 3>&>().at(0))>>);

        SUCCEED();
    }

    TEST(array_methods, at_const_overload_is_not_noexcept) {
        static_assert(!noexcept(std::declval<const array<int, 3>&>().at(0)));

        SUCCEED();
    }

    TEST(array_methods, at_const_overload_does_not_throw) {
        constexpr array<int, 3> values = {1, 2, 3};
        static_assert(1 == values.at(0));
        static_assert(2 == values.at(1));
        static_assert(3 == values.at(2));

        constexpr array<int, 3> runtime_values = {1, 2, 3};
        EXPECT_NO_THROW(static_cast<void>(runtime_values.at(0)));
        EXPECT_NO_THROW(static_cast<void>(runtime_values.at(2)));
        EXPECT_THAT(&runtime_values.at(2), Eq(&runtime_values[2]));
    }

    TEST(array_methods, at_const_overload_throws) {
        // An out-of-range `at` in a constant expression is a compile error, not a catchable throw,
        // so the out-of-range path is only testable at runtime.
        constexpr array<int, 3> values = {1, 2, 3};

        EXPECT_THROW(static_cast<void>(values.at(3)), std::out_of_range);
        EXPECT_THROW(static_cast<void>(values.at(std::numeric_limits<array<int, 3>::size_type>::max())),
                     std::out_of_range);
    }

    TEST(array_methods, at_noexcept_mut_overload_expected_return_value) {
        static_assert(noexcept(std::declval<array<int, 3>&>().at_noexcept(0)));

        array<int, 3> values = {1, 2, 3};

        const auto result = values.at_noexcept(0);
        ASSERT_TRUE(result.has_value());
        EXPECT_THAT(result->get(), Eq(1));
        EXPECT_THAT(&result->get(), Eq(&values[0]));

        // The referenced element is mutable through the wrapper.
        result->get() = 10;
        EXPECT_THAT(values[0], Eq(10));

        // Usable in constant evaluation.
        static_assert([] -> bool {
            array<int, 3> inner = {1, 2, 3};
            const auto ok = inner.at_noexcept(2);
            return ok.has_value() && 3 == ok->get();
        }());
    }

    TEST(array_methods, at_noexcept_mut_overload_unexpected_return_value) {
        array<int, 3> values = {1, 2, 3};

        constexpr auto result = values.at_noexcept(3);
        ASSERT_FALSE(result.has_value());
        EXPECT_THAT(result.error(), Eq(array<int, 3>::array_error::out_of_range));

        constexpr auto far_result = values.at_noexcept(std::numeric_limits<array<int, 3>::size_type>::max());
        ASSERT_FALSE(far_result.has_value());
        EXPECT_THAT(far_result.error(), Eq(array<int, 3>::array_error::out_of_range));

        // The failure path must not throw, which is the entire point of this overload.
        EXPECT_NO_THROW(static_cast<void>(values.at_noexcept(3)));
    }

    TEST(array_methods, at_noexcept_const_overload_expected_return_value) {
        static_assert(noexcept(std::declval<const array<int, 3>&>().at_noexcept(0)));

        static constexpr array<int, 3> values = {1, 2, 3};   // ← static
        constexpr auto result = values.at_noexcept(0);       // now &values.values_[0] is a constant

        ASSERT_TRUE(result.has_value());
        EXPECT_THAT(result->get(), Eq(1));
        EXPECT_THAT(&result->get(), Eq(&values[0]));

        static_assert(std::is_const_v<std::remove_reference_t<decltype(result->get())>>);
        static_assert(result.has_value());
        static_assert(2 == values.at_noexcept(1)->get());
    }

    TEST(array_methods, at_noexcept_const_overload_unexpected_return_value) {
        constexpr array<int, 3> values = {1, 2, 3};

        constexpr auto result = values.at_noexcept(3);
        ASSERT_FALSE(result.has_value());
        EXPECT_THAT(result.error(), Eq(array<int, 3>::array_error::out_of_range));

        EXPECT_NO_THROW(static_cast<void>(values.at_noexcept(3)));

        constexpr array<int, 3> constant_values = {1, 2, 3};
        static_assert(!constant_values.at_noexcept(3).has_value());
    }

    TEST(array_methods, front_mut_overload_single_element_array) {
        static_assert(std::same_as<decltype(std::declval<array<int, 1>&>().front()), array<int, 1>::reference>);
        static_assert(noexcept(std::declval<array<int, 1>&>().front()));

        static_assert([] {
            array<int, 1> values = {1};
            values.front() = 10;
            return 10 == values.front() && 10 == values[0];
        }());

        array<int, 1> values = {1};
        EXPECT_THAT(values.front(), Eq(1));
        EXPECT_THAT(&values.front(), Eq(&values[0]));

        // With one element, front and back are the same object.
        EXPECT_THAT(&values.front(), Eq(&values.back()));

        values.front() = 10;
        EXPECT_THAT(values[0], Eq(10));
    }

    TEST(array_methods, front_mut_overload_multi_element_array) {
        static_assert(std::same_as<decltype(std::declval<array<int, 3>&>().front()), array<int, 3>::reference>);
        static_assert(noexcept(std::declval<array<int, 3>&>().front()));

        static_assert([] {
            array<int, 3> values = {1, 2, 3};
            values.front() = 10;
            return 10 == values.front() && 10 == values[0] && 2 == values[1] && 3 == values[2];
        }());

        array<int, 3> values = {1, 2, 3};
        EXPECT_THAT(values.front(), Eq(1));
        EXPECT_THAT(&values.front(), Eq(&values[0]));
        EXPECT_THAT(&values.front(), Ne(&values.back()));

        values.front() = 10;
        EXPECT_THAT(values[0], Eq(10));
        EXPECT_THAT(values[1], Eq(2));
        EXPECT_THAT(values[2], Eq(3));
    }

    TEST(array_methods, front_const_overload_single_element_array) {
        static_assert(std::same_as<decltype(std::declval<const array<int, 1>&>().front()), array<int, 1>::const_reference>);
        static_assert(std::is_const_v<std::remove_reference_t<decltype(std::declval<const array<int, 1>&>().front())>>);
        static_assert(noexcept(std::declval<const array<int, 1>&>().front()));

        constexpr array<int, 1> constant_values = {1};
        static_assert(1 == constant_values.front());
        static_assert(constant_values.front() == constant_values.back());

        constexpr array<int, 1> values = {1};
        EXPECT_THAT(values.front(), Eq(1));
        EXPECT_THAT(&values.front(), Eq(&values[0]));
        EXPECT_THAT(&values.front(), Eq(&values.back()));
    }

    TEST(array_methods, front_const_overload_multi_element_array) {
        static_assert(std::same_as<decltype(std::declval<const array<int, 3>&>().front()), array<int, 3>::const_reference>);
        static_assert(std::is_const_v<std::remove_reference_t<decltype(std::declval<const array<int, 3>&>().front())>>);
        static_assert(noexcept(std::declval<const array<int, 3>&>().front()));

        constexpr array<int, 3> constant_values = {1, 2, 3};
        static_assert(1 == constant_values.front());
        static_assert(constant_values.front() == constant_values[0]);

        constexpr array<int, 3> values = {1, 2, 3};
        EXPECT_THAT(values.front(), Eq(1));
        EXPECT_THAT(&values.front(), Eq(&values[0]));
        EXPECT_THAT(&values.front(), Ne(&values.back()));
    }

    TEST(array_methods, back_mut_overload_single_element_array) {
        static_assert(std::same_as<decltype(std::declval<array<int, 1>&>().back()), array<int, 1>::reference>);
        static_assert(noexcept(std::declval<array<int, 1>&>().back()));

        static_assert([] {
            array<int, 1> values = {1};
            values.back() = 10;
            return 10 == values.back() && 10 == values.front();
        }());

        array<int, 1> values = {1};
        EXPECT_THAT(values.back(), Eq(1));
        EXPECT_THAT(&values.back(), Eq(&values[0]));
        EXPECT_THAT(&values.back(), Eq(&values.front()));

        values.back() = 10;
        EXPECT_THAT(values[0], Eq(10));
    }

    TEST(array_methods, back_mut_overload_multi_element_array) {
        static_assert(std::same_as<decltype(std::declval<array<int, 3>&>().back()), array<int, 3>::reference>);
        static_assert(noexcept(std::declval<array<int, 3>&>().back()));

        static_assert([] {
            array<int, 3> values = {1, 2, 3};
            values.back() = 30;
            return 30 == values.back() && 1 == values[0] && 2 == values[1] && 30 == values[2];
        }());

        array<int, 3> values = {1, 2, 3};

        // back() must be N-1, not N. This is the off-by-one that a value-only check would miss.
        EXPECT_THAT(values.back(), Eq(3));
        EXPECT_THAT(&values.back(), Eq(&values[2]));
        EXPECT_THAT(&values.back() - &values.front(), Eq(2));

        values.back() = 30;
        EXPECT_THAT(values[0], Eq(1));
        EXPECT_THAT(values[1], Eq(2));
        EXPECT_THAT(values[2], Eq(30));
    }

    TEST(array_methods, back_const_overload_single_element_array) {
        static_assert(std::same_as<decltype(std::declval<const array<int, 1>&>().back()), array<int, 1>::const_reference>);
        static_assert(std::is_const_v<std::remove_reference_t<decltype(std::declval<const array<int, 1>&>().back())>>);
        static_assert(noexcept(std::declval<const array<int, 1>&>().back()));

        constexpr array<int, 1> constant_values = {1};
        static_assert(1 == constant_values.back());

        constexpr array<int, 1> values = {1};
        EXPECT_THAT(values.back(), Eq(1));
        EXPECT_THAT(&values.back(), Eq(&values[0]));
        EXPECT_THAT(&values.back(), Eq(&values.front()));
    }

    TEST(array_methods, back_const_overload_multi_element_array) {
        static_assert(std::same_as<decltype(std::declval<const array<int, 3>&>().back()), array<int, 3>::const_reference>);
        static_assert(std::is_const_v<std::remove_reference_t<decltype(std::declval<const array<int, 3>&>().back())>>);
        static_assert(noexcept(std::declval<const array<int, 3>&>().back()));

        constexpr array<int, 3> constant_values = {1, 2, 3};
        static_assert(3 == constant_values.back());
        static_assert(constant_values.back() == constant_values[2]);

        constexpr array<int, 3> values = {1, 2, 3};
        EXPECT_THAT(values.back(), Eq(3));
        EXPECT_THAT(&values.back(), Eq(&values[2]));
        EXPECT_THAT(&values.back() - &values.front(), Eq(2));
    }
} // namespace collections::array_testing