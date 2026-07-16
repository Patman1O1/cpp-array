// ISO C++ Includes
#include <array> // For tests

// Google Test Includes
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// Local Includes
#include <collections/array.hpp>

namespace collections::array_testing {
    // ── Constructor Tests ────────────────────────────────────────────────────────────────────────────────────────────
    TEST(array_constructors, variadic_overload) {
        // Assert at compile time the operator is noexcept as declared
        static_assert(noexcept(std::declval<array<int, 3>&>()[0]));

        // Assert at compile time that the operator is usable as a constant expression
        constexpr array<int, 3> array = {1, 2, 3};
        static_assert(1 == array[0]);
        static_assert(2 == array[1]);
        static_assert(3 == array[2]);
    }

    // ── Overloaded Operators Tests ───────────────────────────────────────────────────────────────────────────────────
    TEST(array_operators, random_access_mut_overload) {
        // Assert at compile time the operator's return type is a mutable reference
        static_assert(std::same_as<decltype(std::declval<array<int, 3>&>()[0]), array<int, 3>::reference>);

        // Assert at compile time the operator is noexcept as declared
        static_assert(noexcept(std::declval<array<int, 3>&>()[0]));

        // Assert at compile time that the operator is usable as a constant expression
        constexpr array<int, 3> array = {1, 2, 3};
        static_assert(1 == array[0]);
        static_assert(2 == array[1]);
        static_assert(3 == array[2]);
    }

    TEST(array_operators, random_access_const_overload) {
        // Assert at compile time the operator's return type is a constant reference
        static_assert(std::same_as<decltype(std::declval<const array<int, 3>&>()[0]), array<int, 3>::const_reference>);

        // Assert at compile time the operator is noexcept as declared
        static_assert(noexcept(std::declval<const array<int, 3>&>()[0]));

        // Assert at compile time that the operator is usable as a constant expression
        constexpr array<int, 3> array = {1, 2, 3};
        static_assert(1 == array[0]);
        static_assert(2 == array[1]);
        static_assert(3 == array[2]);
    }

    // ── Method Tests ─────────────────────────────────────────────────────────────────────────────────────────────────
    TEST(array_methods, at_mut_overload_does_not_throw) {
        // TODO

    }

    TEST(array_methods, at_mut_overload_throws) {
        // TODO

    }

    TEST(array_methods, at_const_overload_does_not_throw) {
        // TODO
    }

    TEST(array_methods, at_const_overload_throws) {
        // TODO
    }

    TEST(array_methods, at_noexcept_mut_overload_expected_return_value) {
        // TODO
    }

    TEST(array_methods, at_noexcept_mut_overload_unexpected_return_value) {
        // TODO
    }

    TEST(array_methods, at_noexcept_const_overload_expected_return_value) {
        // TODO
    }

    TEST(array_methods, at_noexcept_const_overload_unexpected_return_value) {
        // TODO
    }

    TEST(array_methods, front_mut_overload_single_element_array) {
        // TODO
    }

    TEST(array_methods, front_mut_overload_multi_element_array) {
        // TODO
    }

    TEST(array_methods, front_const_overload_single_element_array) {
        // TODO
    }

    TEST(array_methods, front_const_overload_multi_element_array) {
        // TODO
    }

    TEST(array_methods, back_mut_overload_single_element_array) {
        // TODO
    }

    TEST(array_methods, back_mut_overload_multi_element_array) {
        // TODO
    }

    TEST(array_methods, back_const_overload_single_element_array) {
        // TODO
    }

    TEST(array_methods, back_const_overload_multi_element_array) {
        // TODO
    }

} // namespace collections::array_testing
